好的，根据您提供的 UaStack SDK 关键函数文档和客户端应用程序的生命周期伪代码，我将撰写一份技术文档，详细描述 OPC UA 客户端应用（使用 UaStack）的 **初始化、会话管理和资源清理** 流程。

-----

# 📄 UaStack 客户端应用生命周期与核心 API 文档

## 概述

本文件描述了使用 UaStack SDK 开发 OPC UA 客户端应用程序的完整生命周期，包括初始化底层平台、建立会话连接、执行同步操作、管理订阅以及最终的资源清理。

## 一、 应用程序初始化与清理（全局）

客户端应用程序在开始使用 OPC UA 功能之前，必须初始化 SDK 的底层组件和 XML 解析器。这些操作通常在程序入口点 (`main` 函数) 的开始和结束时调用。

### A. XML 解析器管理

| 函数 | 类别 | 描述 |
| :--- | :--- | :--- |
| `UaXmlDocument::initParser()` | Static | **初始化 XML 解析器。** 必须在任何 XML 相关功能（如加载配置或证书）被使用前调用。 |
| `UaXmlDocument::cleanupParser()` | Static | **清理 XML 解析器资源。** 必须在程序退出前调用，确保资源释放。 |

### B. 平台层管理

| 函数 | 类别 | 描述 |
| :--- | :--- | :--- |
| `UaPlatformLayer::init()` | Static | **初始化 UA Stack 平台层。** 负责分配平台资源和初始化线程等底层组件。任何 `OpcUa_xxx` 核心函数的使用都依赖于此方法的成功调用。 |
| `UaPlatformLayer::cleanup()` | Static | **清理 UA Stack 平台层。** 在程序结束时调用，释放所有平台相关的资源。 |

## 二、 `UaSession`：会话与连接管理

`UaSession` 类是客户端与 OPC UA 服务器交互的核心接口。它负责管理连接的建立、安全设置以及高级服务调用。

### A. 连接与断开

| 函数 | 描述 | 关键参数 |
| :--- | :--- | :--- |
| `UaSession::connect()` | **建立 OPC UA 会话。** 客户端与服务器之间逻辑连接的起点。 | **`sURL` (Server Endpoint):** 服务器地址。 <br> **`sessionConnectInfo`:** 用户凭证、超时设置等。 <br> **`sessionSecurityInfo`:** 安全模式、消息策略等。 <br> **`pSessionCallback`:** 用于接收会话状态变化的异步回调。 |
| `UaSession::disconnect()` | **断开客户端与服务器的连接。** | **`serviceSettings`:** 服务调用设置。 <br> **`bDeleteSubscriptions`:** **重要！** 如果设置为 `OpcUa_True`，服务器将自动删除该会话下所有活动的订阅。 |

## 三、 同步服务调用

一旦会话建立，客户端就可以通过 `UaSession` 对象执行同步的 OPC UA 服务，如数据读写和方法调用。

| 函数 | 服务类型 | 描述 |
| :--- | :--- | :--- |
| `UaSession::read()` | **读服务 (Read)** | 同步读取一个或多个节点属性（通常是 Value 属性）的值、状态和时间戳。 |
| `UaSession::write()` | **写服务 (Write)** | 同步写入一个或多个节点的值。 |
| `UaSession::call()` | **方法调用 (Call)** | 同步调用服务器对象上暴露的 OPC UA 方法。 |

## 四、 异步服务：订阅管理

订阅是 OPC UA 中实现数据或事件实时更新的关键机制。

| 函数 | 作用 | 关键参数 |
| :--- | :--- | :--- |
| `UaSession::createSubscription()` | **创建订阅。** 在服务器上建立一个周期性的数据发布容器。 | **`pUaSubscriptionCallback`:** 客户端实现的回调接口，用于接收异步通知。 <br> **`subscriptionSettings`:** 定义发布间隔 (`PublishingInterval`)。 <br> **`ppUaSubscription`:** 返回新创建的 `UaSubscription` 对象的指针。 |
| `UaSession::deleteSubscription()` | **删除订阅。** 销毁服务器上的订阅实体及其所有监控项。 | **`ppUaSubscription`:** 要删除的订阅对象指针。 |

-----

## 五、 UaStack 客户端生命周期（伪代码流程）

以下是客户端应用程序必须遵循的严格初始化和清理顺序：

```c++
int main()
{
    // -------------------------------------------------------------------
    // 阶段 I: 全局初始化 (Initialization)
    // 必须在任何 UaStack 或 XML 调用之前完成
    // -------------------------------------------------------------------
    UaXmlDocument::initParser();    // 初始化 XML 解析器
    UaPlatformLayer::init();        // 初始化 UaStack 底层平台

    // -------------------------------------------------------------------
    // 阶段 II: 会话管理与服务交互 (Session & Interaction)
    // -------------------------------------------------------------------
    // 1. 建立会话连接
    UaSession::connect();

    // 2. 数据同步交互
    UaSession::read();
    UaSession::write();
    UaSession::call();
    
    // 3. 订阅管理
    UaSubscription* pSub = NULL;
    UaSession::createSubscription(..., &pSub); // 创建订阅并接收异步回调
    // ... 添加 MonitoredItems 并等待数据 ...
    UaSession::deleteSubscription(..., &pSub); // 删除订阅

    // -------------------------------------------------------------------
    // 阶段 III: 资源清理 (Cleanup)
    // 必须以与初始化相反的顺序进行，确保依赖关系被正确释放
    // -------------------------------------------------------------------
    UaSession::disconnect();        // 断开会话连接
    UaPlatformLayer::cleanup();     // 清理 UaStack 底层平台
    UaXmlDocument::cleanupParser(); // 清理 XML 解析器
}
```
