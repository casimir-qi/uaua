python 将当前目录作为http服务端目录，端口号为8008
nohup python -m http.server 80 > output.log 2>&1 &

在 Linux 中，如果你想将一个命令从当前终端脱离并让它在后台运行，可以使用以下几种方法：

### 1. 使用 `&` 将命令放到后台
在命令末尾加上 `&` 可以让命令在后台运行。

例如：

```bash
your-command &
```

这会使 `your-command` 在后台运行，并且返回一个进程 ID (PID)。

### 2. 使用 `nohup` 保持命令在后台运行，即使退出终端
`nohup` (no hang up) 使得命令在当前终端关闭后仍继续运行。

例如：

```bash
nohup your-command &
```

这会将命令放到后台并且即使你关闭终端，它依然会继续执行。输出会默认写入 `nohup.out` 文件，除非你指定其他输出文件。

如果你想指定输出文件，可以使用：

```bash
nohup your-command > output.log 2>&1 &
```

### 3. 使用 `disown` 命令
你可以先将一个命令放到后台运行，然后使用 `disown` 将其从当前 shell 中分离，使其不再与当前终端关联。

步骤如下：

1. 将命令放到后台：

   ```bash
   your-command &
   ```

2. 使用 `disown` 命令：

   ```bash
   disown
   ```

这将移除后台进程与当前终端的关联，即使你关闭终端，进程也会继续运行。

### 4. 使用 `screen` 或 `tmux` 启动会话
如果你需要一个交互式的会话，并且想要在终端关闭后仍然能够恢复进程，可以使用 `screen` 或 `tmux`。

- 启动 `screen` 会话：

  ```bash
  screen
  ```

  然后你可以在里面运行命令。要退出 `screen` 会话并保持进程运行，可以按 `Ctrl-A` 然后按 `D`。

- 启动 `tmux` 会话：

  ```bash
  tmux
  ```

  然后你可以在里面运行命令。要退出 `tmux` 会话并保持进程运行，按 `Ctrl-B` 然后按 `D`。

使用这些工具可以让你更方便地管理在后台运行的任务，并且能够随时恢复对它们的控制。

这些方法都可以帮助你将命令脱离当前终端继续运行。

你可以使用 Python 内置的 `http.server` 模块来快速启动一个 HTTP 服务器，将当前目录作为服务端的根目录，端口号为 8008。

在命令行中执行以下命令：

```bash
python -m http.server 8008
```

这会启动一个 HTTP 服务，将当前工作目录作为根目录，监听 8008 端口。

如果你想在代码中实现，你可以用以下 Python 脚本：

```python
import http.server
import socketserver

PORT = 8008

Handler = http.server.SimpleHTTPRequestHandler

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    print(f"Serving at port {PORT}")
    httpd.serve_forever()
```

这个脚本会启动一个 HTTP 服务器，并且将当前目录作为服务目录，监听 8008 端口。你可以通过访问 `http://localhost:8008/` 来查看当前目录下的文件。