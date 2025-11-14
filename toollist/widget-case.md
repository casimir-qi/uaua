对于修改列配置的表格设计要求，字段名通过配置获取，不可以变动。字段的属性可以修改。字段所在列可以变动。设计一个qt控件实现

好的，根据您的要求，我将设计一个基于 Qt 控件的界面实现方案。这个方案允许用户修改列的**属性**（是否显示、列宽等），并调整列的**顺序**（字段所在列可以变动），但**字段名**本身是固定的，只能通过配置获取和展示。

我们将使用 `QTableWidget` 作为核心控件，因为它非常适合展示和重新排序表格化的数据，并且提供了方便的编辑功能。

### 📊 Qt 控件设计方案：`ColumnConfigWidget`

我们将设计一个自定义的 `QWidget`，例如命名为 `ColumnConfigWidget`。

#### 1\. 核心控件：`QTableWidget`

使用一个 `QTableWidget` 来展示所有可配置的列，每一行代表配置文件中的一个 `columnX` 配置项。

**`QTableWidget` 的列定义：**

| 序号 | 标题 | 对应配置参数 | 交互类型 | 备注 |
| :--- | :--- | :--- | :--- | :--- |
| **0** | **字段名 (Field)** | 字段名 (第 1 部分) | **只读文本** | **用户不可修改**，通过配置加载。 |
| **1** | **数据类型** | 数据类型 (第 2 部分) | 可编辑文本/下拉框 | 属性修改。 |
| **2** | **提示标志** | 提示信息标志 (第 3 部分) | 可编辑文本 | 属性修改。 |
| **3** | **是否显示** | 是否显示 (第 4 部分) | **复选框 (`QCheckBox`)** | 属性修改，直观控制可见性。 |
| **4** | **列宽 (px)** | 列宽 (第 5 部分) | **`QSpinBox`** | 属性修改，限制输入范围。 |
| **5** | **操作** | 无 | 按钮/图标 | 用于上下移动列（调整顺序）。 |

-----

#### 2\. 实现关键需求

##### A. 列属性修改（可变动）

在 `QTableWidget` 中，需要将对应的单元格设置为可编辑的自定义控件：

  * **是否显示**：使用 `QTableWidgetItem` 配合 `Qt::ItemIsUserCheckable` 标志，或者更直观地，使用 `QWidget::setCellWidget` 将一个 `QCheckBox` 嵌入到单元格中。
  * **列宽**：使用 `QWidget::setCellWidget` 将一个 `QSpinBox` 嵌入到单元格中，设置范围限制（如 50-500）。

##### B. 字段名固定（不可变动）

  * **字段名单元格 (第 0 列)**：设置 `Qt::ItemIsEditable` 标志为 `false`，确保用户不能修改名称。

##### C. 字段所在列变动（拖拽或按钮）

这是最核心的功能。有两种实现方式：

1.  **上下移动按钮（推荐，实现简单）：**

      * 在 **操作** 列（第 5 列）为每一行嵌入两个 `QPushButton` (↑ 和 ↓ 图标)。
      * 点击 ↑ 按钮，将当前行的数据与上一行的数据**交换**。
      * 点击 ↓ 按钮，将当前行的数据与下一行的数据**交换**。
      * **注意：** 交换的只是**数据内容**（字段名、属性），而不是 `columnX` 的序号。在保存时，新的顺序会对应到新的 `column1, column2, ...`。

2.  **行拖拽 (Drag & Drop)：**

      * 在 `QTableWidget` 上启用拖拽功能 (`setDragDropMode(QAbstractItemView::InternalMove)`)。
      * 用户可以直接拖动行来改变顺序。这在交互上更流畅，但需要处理拖拽后的数据同步。

### 3\. 界面布局示例 (伪代码)

```cpp
// ColumnConfigWidget.h
class ColumnConfigWidget : public QWidget {
    Q_OBJECT
public:
    explicit ColumnConfigWidget(QWidget *parent = nullptr);
    void loadConfig(const QString &iniPath);
    void saveConfig(const QString &iniPath);

private slots:
    void moveRowUp();
    void moveRowDown();

private:
    QTableWidget *tableWidget;
    // 存储从配置文件中加载的原始列数据 (List of QStringList, 方便重新排序)
    QList<QStringList> columnDataList; 
};

// ColumnConfigWidget.cpp (加载部分)
void ColumnConfigWidget::loadConfig(const QString &iniPath) {
    // ... QSettings 加载逻辑 ...
    
    // 假设从配置文件中得到了 24 列数据
    tableWidget->setRowCount(24);
    
    for (int i = 0; i < 24; ++i) {
        // 1. 字段名 (Field) - 不可编辑
        QTableWidgetItem *fieldItem = new QTableWidgetItem(columnDataList[i][0]);
        fieldItem->setFlags(fieldItem->flags() & ~Qt::ItemIsEditable); // 设置为不可编辑
        tableWidget->setItem(i, 0, fieldItem);

        // 2. 是否显示 (Display) - 使用 CheckBox 控件
        QCheckBox *displayBox = new QCheckBox();
        displayBox->setChecked(columnDataList[i][3] == "1");
        tableWidget->setCellWidget(i, 3, displayBox); 

        // 3. 列宽 (Width) - 使用 SpinBox 控件
        QSpinBox *widthSpin = new QSpinBox();
        widthSpin->setRange(30, 500);
        widthSpin->setValue(columnDataList[i][4].toInt());
        tableWidget->setCellWidget(i, 4, widthSpin);
        
        // 4. 操作 (Movement Buttons) - 使用自定义 Widget 嵌入按钮
        QWidget *buttonWidget = new QWidget();
        QHBoxLayout *hLayout = new QHBoxLayout(buttonWidget);
        QPushButton *upBtn = new QPushButton("↑");
        QPushButton *downBtn = new QPushButton("↓");
        hLayout->addWidget(upBtn);
        hLayout->addWidget(downBtn);
        hLayout->setContentsMargins(0, 0, 0, 0);
        tableWidget->setCellWidget(i, 5, buttonWidget);
        
        // 连接信号到槽函数，处理移动逻辑
        connect(upBtn, &QPushButton::clicked, this, &ColumnConfigWidget::moveRowUp);
        // ... connect downBtn ...
    }
}
```

#### 4\. 保存逻辑（关键）

保存时，程序需要**按照 `QTableWidget` 中当前的行顺序**重新生成 `column1`, `column2`, `column3`... 的键值对，而不是按字段名来排序。

```cpp
// ColumnConfigWidget.cpp (保存部分)
void ColumnConfigWidget::saveConfig(const QString &iniPath) {
    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup("toollisttable");
    
    int visibleCount = 0;
    int rowCount = tableWidget->rowCount();
    
    for (int i = 0; i < rowCount; ++i) {
        // 从 tableWidget 的第 i 行（当前顺序）获取配置数据
        QString fieldName = tableWidget->item(i, 0)->text(); 
        
        // 获取控件中的属性值
        QCheckBox *displayBox = qobject_cast<QCheckBox*>(tableWidget->cellWidget(i, 3));
        QSpinBox *widthSpin = qobject_cast<QSpinBox*>(tableWidget->cellWidget(i, 4));
        
        QString displayValue = displayBox->isChecked() ? "1" : "0";
        if (displayBox->isChecked()) {
            visibleCount++;
        }
        
        QString widthValue = QString::number(widthSpin->value());
        
        // ******* 关键： i+1 决定了 columnX 的序号 *******
        QString newKey = QString("column%1").arg(i + 1); 
        
        // 重构字符串：字段名$类型$提示$显示$列宽
        // 假设类型和提示信息标志也从对应的单元格获取
        QString newConfig = QString("%1$%2$%3$%4$%5")
                            .arg(fieldName)
                            .arg("5") // 示例：从控件获取类型
                            .arg("tips") // 示例：从控件获取提示
                            .arg(displayValue)
                            .arg(widthValue);
        
        settings.setValue(newKey, newConfig);
    }
    
    // 更新 table=N$M
    settings.setValue("table", QString("%1$%2").arg(rowCount).arg(visibleCount));
    settings.endGroup();
}
```