根据我们前面分析的代码片段和 OPC UA 的规范，撰写一份关于使用 UaStack SDK 实现 **OPC UA 事件订阅** 功能的详细文档。

这份文档将侧重于 **事件过滤器（Event Filter）** 的构建，这是事件订阅中最复杂也是最关键的部分。

-----

# 📝 UaStack SDK 实现 OPC UA 事件订阅功能文档

## 概述

OPC UA 事件订阅功能允许客户端监控 OPC UA 服务器上的特定事件源（如 `Server` 对象或特定的对象实例），并在满足特定条件时接收事件通知。与数据订阅不同，事件订阅的关键在于构建复杂的 **事件过滤器（Event Filter）** 来定义要返回的事件类型和数据字段。

在 UaStack SDK 中，事件订阅通过向 `createMonitoredItems` 服务发送一个特殊的监控项请求来实现。

## 一、 核心概念：事件过滤器 (`UaEventFilter`)

事件过滤器由两部分组成，共同决定了最终推送给客户端的事件内容：

| 部分 | UaStack 对象 | 核心作用 (SQL 对应) |
| :--- | :--- | :--- |
| **选择子句** | `UaSimpleAttributeOperands` | **投影 (SELECT)**：定义事件通知中应包含哪些数据字段。 |
| **过滤条件** | `UaContentFilter` | **筛选 (WHERE)**：定义事件必须满足的逻辑条件。 |

## 二、 实现步骤：创建事件监控项

### 步骤 1: 创建订阅 (Subscription)

与数据订阅相同，首先必须创建一个基础的 `UaSubscription` 实例。

### 步骤 2: 构建事件监控项请求

事件监控项请求与数据监控项请求（`MonitoredItemCreateRequest`）的区别在于 `ItemToMonitor` 和 `Filter` 参数。

| 请求参数 | 设置值 | 备注 |
| :--- | :--- | :--- |
| `ItemToMonitor.NodeId` | `OpcUaId_Server` | 通常订阅服务器根节点（或其他支持事件通知的节点）。 |
| `ItemToMonitor.AttributeId`| `OpcUa_Attributes_EventNotifier` | **必须设置** 为事件通知属性。 |
| `SamplingInterval` | `0` | **必须设置** 为 0。事件的触发是基于服务器事件发生，而不是周期采样。 |
| `QueueSize` | `0` 或更大 | 队列大小通常为 0，表示只保留最新的事件。 |

### 步骤 3: 定义选择子句 (SELECT Clauses)

选择子句定义了每个事件通知中客户端想要接收的属性列表。

```cpp
// 1. 创建 UaEventFilter 实例
UaEventFilter eventFilter;

// 2. 定义选择列表容器并预设大小（例如 5 个字段）
UaSimpleAttributeOperands selectClauses;
selectClauses.create(5);

// 3. 定义并添加字段：使用 UaSimpleAttributeOperand 和 BrowsePath
UaSimpleAttributeOperand selectElement;

// 示例：添加标准字段 Message (ns=0)
selectElement.setBrowsePathElement(0, UaQualifiedName("Message", 0), 1);
selectElement.copyTo(&selectClauses[0]);

// 示例：添加自定义字段 Temperature (假设 ns=3)
selectElement.setBrowsePathElement(0, UaQualifiedName("Temperature", 3), 1); 
selectElement.copyTo(&selectClauses[3]);

// 4. 设置选择子句
eventFilter.setSelectClauses(selectClauses);
```

### 步骤 4: 定义过滤条件子句 (WHERE Clause)

过滤条件子句 (`UaContentFilter`) 是一个复杂的逻辑表达式树，用于筛选事件。它由 `UaContentFilterElement` 数组构成，元素之间通过索引 (`UaElementOperand`) 互相引用。

**目标过滤器表达式示例：** $( \text{Severity} > 100 ) \quad \text{AND} \quad ( \text{OfType}(\text{ControllerEventType}) \quad \text{OR} \quad \text{OfType}(\text{OffNormalAlarmType}) )$

```cpp
// 1. 创建 UaContentFilter (Where 子句的根)
UaContentFilter* pContentFilter = new UaContentFilter;

// 2. 构建元素 [1] Severity > 100
// ... 使用 GreaterThan, UaSimpleAttributeOperand ("Severity"), UaLiteralOperand (100) ...
pContentFilter->setContentFilterElement(1, pContentFilterElement_1, 5); // **重要：所有权转移**

// 3. 构建元素 [3] OfType(ControllerEventType) 和 [4] OfType(OffNormalAlarmType)
// ... 使用 OfType, UaLiteralOperand (NodeId of event type) ...
pContentFilter->setContentFilterElement(3, pContentFilterElement_3, 5);
pContentFilter->setContentFilterElement(4, pContentFilterElement_4, 5);

// 4. 构建元素 [2] [3] OR [4] (类型逻辑)
// ... 使用 Or, UaElementOperand (引用索引 3 和 4) ...
pContentFilter->setContentFilterElement(2, pContentFilterElement_2, 5);

// 5. 构建元素 [0] [1] AND [2] (根逻辑)
// ... 使用 And, UaElementOperand (引用索引 1 和 2) ...
pContentFilter->setContentFilterElement(0, pContentFilterElement_0, 5);

// 6. 将 Where 子句设置给 EventFilter
eventFilter.setWhereClause(pContentFilter); // **重要：UaEventFilter 接管 pContentFilter 的内存所有权**
```

### 步骤 5: 附加过滤器并创建监控项

将构建好的 `UaEventFilter` 序列化为 `ExtensionObject`，并附加到监控项创建请求中。

```cpp
// Detach EventFilter 到监控项创建请求的 Filter 字段
eventFilter.toExtensionObject(
    monitoredItemCreateRequests[count].RequestedParameters.Filter, 
    OpcUa_True
);

// 最后，调用 createMonitoredItems 服务
status = pUaSubscription->createMonitoredItems(
    serviceSettings, 
    OpcUa_TimestampsToReturn_Both, 
    monitoredItemCreateRequests, 
    monitoredItemCreateResults
);
```

## 三、 事件通知处理（回调函数）

与数据变更通知类似，事件通知通过订阅回调函数接收。

**关键函数：** `UaSubscriptionCallback::newEvents`

客户端需要实现此虚函数来接收事件。接收到的数据通常是一个 `UaVariant` 数组，其顺序和内容严格遵循您在 **选择子句** 中定义的字段顺序。客户端需要根据 `ClientHandle` 识别事件源，并按顺序解析返回的字段值。

-----



# 过滤条件子句（whereClauses）基本概念 
想要自定义 whereClauses,需要知道的基本概念和类有哪些
要自定义 OPC UA 事件过滤器中的 `Where` 子句（即 `UaContentFilter`），您需要掌握以下几个基本概念和类。这是 OPC UA 客户端编程中逻辑最复杂的部分之一，因为它涉及构建一个 **逻辑表达式树**。

## 🎯 核心概念：逻辑表达式树

`Where` 子句的本质是一个逻辑表达式树，其中：

  * **节点 (Nodes)** 是操作符（AND, OR, GreaterThan, OfType 等）。
  * **叶子 (Leaves)** 是操作数（常量值、节点属性引用或对其他节点的引用）。

## 🛠️ 关键类和概念总结

| 类别 | 关键类 / 枚举 | 核心概念 | 作用 |
| :--- | :--- | :--- | :--- |
| **过滤器根** | `UaContentFilter` | **Where 子句** | 整个逻辑表达式树的容器。它包含一个 `UaContentFilterElement` 数组。 |
| **逻辑单元** | `UaContentFilterElement` | **过滤器元素** | 表达式树的每个节点。它包含一个 **操作符** 和一组 **操作数**。 |
| **操作符** | `OpcUa_FilterOperator` | **逻辑/比较操作** | 定义该元素执行的操作，如 `And`、`Or`、`GreaterThan`、`OfType` 等。 |
| **操作数** | `UaFilterOperand` (基类) | **操作对象的抽象** | 抽象基类，所有具体的操作数类型都继承自它。 |
| **具体操作数** | `UaLiteralOperand` | **常量值** | 表示表达式中的一个常量值（如数字 `100`，或字符串 `"Critical"`）。 |
| **具体操作数** | `UaSimpleAttributeOperand` | **节点属性引用** | 表示表达式中要比较的事件字段属性（如 `Severity` 严重性、`Message` 消息）。 |
| **具体操作数** | `UaElementOperand` | **元素引用** | 表示对 `UaContentFilter` 数组中 **另一个 `UaContentFilterElement` 索引的引用**。这是构建逻辑树的关键。 |

-----

### 详细解释和使用方法

#### 1\. `UaContentFilter` (Where 子句)

这是最顶层的容器。您需要：

1.  创建一个 `UaContentFilter` 实例。
2.  使用 `setContentFilterElement(index, pElement, ownership)` 方法向其内部数组添加 `UaContentFilterElement`。
3.  **注意：** 您必须在最终的 `UaEventFilter` 中将 `UaContentFilter` 设置为 `Where` 子句：`eventFilter.setWhereClause(pContentFilter);`。

#### 2\. `UaContentFilterElement` (逻辑单元)

每个 `UaContentFilterElement` 代表一个完整的运算单元。

  * **操作符设置：** 使用 `setFilterOperator(operator)` 指定操作，例如 `OpcUa_FilterOperator_And`。
  * **操作数设置：** 使用 `setFilterOperand(index, pOperand, ownership)` 添加参与运算的操作数。

#### 3\. 关键操作数类型

| 类 | 作用 | 创建和使用方式 | 对应代码示例中的字段 |
| :--- | :--- | :--- | :--- |
| `UaLiteralOperand` | **常量** | `pOperand = new UaLiteralOperand;` <br> 然后用 `setLiteralValue(UaVariant(value))` 设置值。 | `Severity > **100**` 中的 `100`。 |
| `UaSimpleAttributeOperand` | **字段引用** | `pOperand = new UaSimpleAttributeOperand;` <br> 然后用 `setBrowsePathElement(...)` 指定要引用的事件字段（如 `"Severity"`）。 | **`Severity`** `> 100` 中的 `Severity`。 |
| `UaElementOperand` | **逻辑引用** | `pOperand = new UaElementOperand;` <br> 然后用 `setIndex(index)` 指定它引用的 `UaContentFilter` 数组中的另一个元素的索引。 | `[1] AND [2]` 中的 `[1]` 和 `[2]`。 |

#### 4\. 内存管理（关键）

在 UaStack 中，涉及到指针参数的函数通常会进行**所有权转移**。

  * 当您调用 `pContentFilter->setContentFilterElement(index, pElement, 5)` 时，第三个参数 `5` 表示 **`UaContentFilter` 接收 `pElement` 的内存所有权**。您无需手动 `delete pElement`。
  * 同样，`eventFilter.setWhereClause(pContentFilter)` 表示 `UaEventFilter` 接收 `pContentFilter` 的所有权。

正确管理所有权是避免内存泄漏和双重释放的关键。

-----

