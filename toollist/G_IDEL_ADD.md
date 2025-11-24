`g_idle_add` 是 GLib 库（GTK 的基础库）中的一个核心函数。它的主要作用是将一个函数添加到 GLib 的 **主循环（Main Loop）** 中，以便在系统“空闲”时（即没有更高优先级的事件，如按键、点击或重绘需要处理时）执行。

它通常用于以下两种场景：

1.  **推迟任务**：将非紧急任务推迟到当前处理完成后立即执行。
2.  **线程通信（最常用）**：在多线程程序中，从**子线程**安全地更新**主线程**（UI 线程）中的界面组件。

-----

### 1\. 函数原型

```c
guint g_idle_add (GSourceFunc function, gpointer data);
```

  * **`function`**: 回调函数，当主循环空闲时会调用它。
  * **`data`**: 传递给回调函数的用户数据（可以是结构体指针、变量等）。
  * **返回值**: 返回一个无符号整数（ID/Tag），用于标识这个事件源。如果需要手动取消该任务，可以使用 `g_source_remove(tag)`。

-----

### 2\. 回调函数的写法

回调函数必须符合 `GSourceFunc` 的定义。**返回值非常重要**：

```c
gboolean my_idle_callback(gpointer data) {
    // 在这里做你的工作...
    
    // 返回 FALSE (G_SOURCE_REMOVE): 函数执行一次后自动从主循环移除（最常用）。
    // 返回 TRUE  (G_SOURCE_CONTINUE): 函数会不断重复执行（类似于 while 循环，要小心 CPU 占用）。
    return FALSE; 
}
```

-----

### 3\. 场景一：基本用法（推迟执行）

这是最简单的用法，让某个函数在当前所有高优先级事件处理完后执行一次。

```c
#include <glib.h>
#include <stdio.h>

// 回调函数
gboolean on_idle_task(gpointer data) {
    char *msg = (char*)data;
    printf("Idle task running: %s\n", msg);
    
    // 返回 FALSE 表示只运行一次
    return FALSE; 
}

int main(int argc, char *argv[]) {
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    printf("Main loop starting...\n");

    // 添加 idle 任务
    g_idle_add(on_idle_task, "Hello from Idle!");

    // 启动主循环
    g_main_loop_run(loop);

    return 0;
}
```

-----

### 4\. 场景二：多线程更新 UI（重点）

这是 `g_idle_add` 最核心的用途。**GTK/GLib 的 UI 操作不是线程安全的**，你**不能**在子线程中直接修改 Label 或 ProgressBar。你必须使用 `g_idle_add` 将更新操作“抛”给主线程去执行。

**工作流程：**

1.  子线程进行耗时计算。
2.  子线程准备好结果。
3.  子线程调用 `g_idle_add`，注册一个更新 UI 的回调函数。
4.  主线程在空闲时执行该回调，安全地更新 UI。

#### 代码示例：

```c
#include <gtk/gtk.h>
#include <pthread.h>

// 用于在线程间传递数据的结构体
typedef struct {
    GtkWidget *label;
    int progress;
} ThreadData;

// 3. 这个函数运行在【主线程】
// 它负责实际更新 UI
gboolean update_ui_callback(gpointer user_data) {
    ThreadData *data = (ThreadData *)user_data;
    
    char buffer[64];
    sprintf(buffer, "Progress: %d%%", data->progress);
    gtk_label_set_text(GTK_LABEL(data->label), buffer);

    // 释放之前分配的内存
    g_free(data);

    // 返回 FALSE，确保只更新一次
    return FALSE; 
}

// 2. 这个函数运行在【子线程】
// 模拟耗时操作
void *worker_thread(void *arg) {
    GtkWidget *label = (GtkWidget *)arg;
    
    for (int i = 0; i <= 100; i += 10) {
        // 模拟耗时工作
        g_usleep(500000); // 0.5秒

        // 准备数据传给主线程
        // 注意：这里必须动态分配内存，因为是跨线程传递
        ThreadData *data = g_new(ThreadData, 1);
        data->label = label;
        data->progress = i;

        // *** 关键点 ***
        // 将 update_ui_callback 加入主循环
        // 这个函数本身是线程安全的，可以在子线程调用
        g_idle_add(update_ui_callback, data);
    }
    return NULL;
}

// 1. 程序入口
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *label = gtk_label_new("Starting...");
    gtk_container_add(GTK_CONTAINER(window), label);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);

    // 创建子线程
    pthread_t tid;
    pthread_create(&tid, NULL, worker_thread, label);
    pthread_detach(tid); // 分离线程，无需 join

    gtk_main();
    return 0;
}
```

-----

### 5\. 注意事项与常见坑

  * **内存管理**：
    在多线程场景下（如上例），传递给 `g_idle_add` 的 `data` 通常需要动态分配（`malloc`/`g_new`），并在回调函数结束前释放（`free`/`g_free`），否则会导致内存泄漏。
  * **返回值陷阱**：
    如果你在回调中错误地返回了 `TRUE`，而你的回调函数里没有阻塞逻辑，程序会变成一个**死循环**（Busy Loop），导致 CPU 占用率飙升至 100%，因为系统会认为“我还想继续运行这个 Idle 任务”。
  * **优先级**：
    `g_idle_add` 的优先级实际上非常低（`G_PRIORITY_DEFAULT_IDLE`）。如果主循环非常繁忙（比如一直在处理大量的鼠标移动事件），Idle 任务可能会被推迟很久。如果需要更高的优先级，请使用 `g_timeout_add` 或 `g_main_context_invoke`。
  * **取消任务**：
    如果你保存了 `g_idle_add` 的返回值（`guint tag`），在任务执行前（或者如果任务返回 TRUE），你可以通过调用 `g_source_remove(tag)` 来取消它。

-----

**我可以为你做什么？**

您是想用它来解决**多线程界面卡顿**的问题，还是有特定的**定时/延时**需求需要实现？告诉我您的具体场景，我可以帮您写一段更贴切的代码。