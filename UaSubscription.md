如何使用 UaStack SDK 实现 OPC UA 的数据更新订阅功能。这份文档结合了您提供的代码片段和我们讨论过的关键概念（如订阅创建、监控项、回调函数等）。

-----

# 📝 UaStack SDK 实现 OPC UA 数据更新订阅功能文档

## 概述

本功能旨在通过 OPC UA 客户端（使用 UaStack SDK）连接到 OPC UA 服务器，创建数据订阅，并监控一组变量的实时数据变化。当被监控的变量值、状态或时间戳发生变化时，服务器将异步地通知客户端。

## 一、 核心概念回顾

在 UaStack 中，实现数据更新订阅涉及三个主要层次：

| 层次 | UaStack 对象 | 作用 |
| :--- | :--- | :--- |
| **会话层** | `UaSession` | 客户端与服务器建立连接的通道。 |
| **订阅层** | `UaSubscription` | 周期性数据发布容器，定义了数据发布的频率 (`PublishingInterval`)。 |
| **监控项层** | `UaMonitoredItem` | 实际监控的变量或属性，定义了采样频率 (`SamplingInterval`) 和数据过滤规则。 |
| **回调机制** | `UaSubscriptionCallback` | 客户端接收服务器异步通知的机制。 |

## 二、 实现步骤与代码解析

### 1\. 创建订阅 (Subscription)

订阅是监控项的容器，它定义了服务器向客户端推送数据的频率。

**关键函数：** `UaSession::createSubscription`

```cpp
UaStatus status;
UaSubscription* pUaSubscription = NULL;
SubscriptionSettings subscriptionSettings;

// 1. 设置订阅参数：数据发布间隔为 500ms
subscriptionSettings.publishingInterval = 500;
ServiceSettings serviceSettings;

// 2. 调用创建订阅服务
status = g_pUaSession->createSubscription(
    serviceSettings,
    g_pCallback,             // UaSubscriptionCallback 实例，用于接收数据
    0,                       // 客户端订阅句柄，此处未使用
    subscriptionSettings,
    OpcUa_True,              // Publishing enabled (启用发布)
    &pUaSubscription);       // 返回的 UaSubscription 实例
// ... (错误检查) ...
```

### 2\. 创建监控项 (Monitored Items)

在已创建的订阅内，定义要监控的具体变量。

**关键结构：** `UaMonitoredItemCreateRequests`

| 参数 | 含义 | 示例值 | 备注 |
| :--- | :--- | :--- | :--- |
| `ItemToMonitor.NodeId` | 监控项的节点 ID | `ns=1;i=1325` | 唯一标识变量。 |
| `ItemToMonitor.AttributeId`| 监控的属性 | `OpcUa_Attributes_Value` | 监控变量的当前值。 |
| `MonitoringMode` | 监控模式 | `OpcUa_MonitoringMode_Reporting` | 模式设置为上报，采样后数据变化则推送到客户端。 |
| `ClientHandle` | 客户端句柄 | `i+1` (递增) | 客户端用于识别数据来自哪个变量的唯一 ID。 |
| `SamplingInterval` | 采样间隔 | `1000` | 服务器检查变量是否有更新的频率（单位：毫秒）。 |

**代码片段：**

```cpp
OpcUa_UInt32 count = 1; 
UaMonitoredItemCreateRequests monitoredItemCreateRequests;
monitoredItemCreateRequests.create(count);

// 目标节点：ns=1;i=1325
UaNodeId targetNodeId(1325, 1); 

// 填充创建请求结构体
monitoredItemCreateRequests[0].ItemToMonitor.AttributeId = OpcUa_Attributes_Value; 
monitoredItemCreateRequests[0].MonitoringMode = OpcUa_MonitoringMode_Reporting;
monitoredItemCreateRequests[0].RequestedParameters.ClientHandle = 1;
monitoredItemCreateRequests[0].RequestedParameters.SamplingInterval = 1000;
// ... (其他参数设置) ...

// **解决类型不匹配的关键步骤**
targetNodeId.copyTo(&monitoredItemCreateRequests[0].ItemToMonitor.NodeId);

// 3. 调用创建监控项服务
status = pUaSubscription->createMonitoredItems(
    serviceSettings,
    OpcUa_TimestampsToReturn_Both,
    monitoredItemCreateRequests,
    monitoredItemCreateResults);
// ... (结果检查和 MonitoredItemId 的存储) ...
```

> **注意：** 在结果检查中，必须存储服务器返回的 **`MonitoredItemId`**，这是后续修改或删除操作的关键标识。

### 3\. 数据更新处理（回调函数）

客户端通过实现 `UaSubscriptionCallback` 接口的 `dataChange` 虚函数来接收数据。

**关键函数：** `SampleSubscription::dataChange`

```cpp
void SampleSubscription::dataChange(
    OpcUa_UInt32               clientSubscriptionHandle,
    const UaDataNotifications& dataNotifications,
    const UaDiagnosticInfos&   diagnosticInfos)
{
    // 遍历服务器发送的所有数据通知
    for ( OpcUa_UInt32 i=0; i<dataNotifications.length(); i++ )
    {
        // 1. 检查状态码是否良好 (OpcUa_IsGood)
        if ( OpcUa_IsGood(dataNotifications[i].Value.StatusCode) )
        {
            // 2. 提取数据
            UaVariant tempValue = dataNotifications[i].Value.Value; 
            // 3. 使用 ClientHandle 识别变量
            printf("  Variable %d value = %s\n", 
                   dataNotifications[i].ClientHandle, 
                   tempValue.toString().toUtf8());
        }
        else
        {
            // 4. 处理错误或警告
            UaStatus itemError(dataNotifications[i].Value.StatusCode);
            printf("  Variable %d failed with status %s\n", 
                   dataNotifications[i].ClientHandle, itemError.toString().toUtf8());
        }
    }
}
```

### 4\. 清理与关闭

在应用程序退出或不需要订阅时，必须正确删除订阅，服务器会自动删除订阅下的所有监控项。

**关键函数：** `UaSession::deleteSubscription`

```cpp
status = g_pUaSession->deleteSubscription(
    serviceSettings,
    &pUaSubscription);
// ... (错误检查) ...
```

## 三、 扩展功能：事件订阅（Event Subscription）

如果要订阅事件而不是数据变量，流程基本相同，但需要注意以下几点：

1.  **ItemToMonitor：** 通常监控 **Server 对象**（NodeId: `OpcUaId_Server`）或具有 `EventNotifier` 属性的节点。
2.  **AttributeId：** 必须设置为 `OpcUa_Attributes_EventNotifier`。
3.  **过滤器：** 需要创建并附加 `UaEventFilter`（包含 `Select` 子句和可选的 `Where` 子句）到请求参数的 `Filter` 字段中，通过 `eventFilter.toExtensionObject()` 实现。

-----

**下一步我可以为您做些什么？**

  * 您希望将这份文档转换为更正式的格式（如 Markdown 或 PDF 模板）吗？
  * 或者，您想在文档中加入关于 **`setTriggering` 或 `modifyMonitoredItems`** 的章节吗？
