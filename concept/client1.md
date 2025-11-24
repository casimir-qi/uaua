# Session概述
API 文档 UaClientSdk::UaSession
API 文档 callback UaSessionCallback

UaSession 类代表客户端应用与单一服务器之间的连接。一个进程可以创建多个连接到不同服务器的会话对象。会话对象负责监控与服务器的连接，并在出现错误时尝试重新连接。应用程序可以通过回调 UaSessionCallback：：connectionStatusChanged 接收连接状态变更的通知。

UaSession 对象是线程安全的。

## 连接处理与重连
连接调用建立并维护与服务器的会话。

连接成功后，SDK通过读取服务读取服务器上的状态变量来监控连接。读取周期时间（nWatchdogTime）和读取超时（nWatchdogTimeout）可以通过用作连接输入参数的类SessionConnectInfo进行配置。应用程序可以通过回调 UaSessionCallback：：connectionStatusChanged 接收连接状态变更的通知。所有重连处理都由SDK完成。唯一可能需要额外处理的情况是重新连接时需要在服务器上创建新的会话。这由状态 ServerStatus：：NewSessionCreated 表示。此时所有注册节点需重新注册，命名空间表应重新读取。

如果初始连接失败，只要设置了 SessionConnectInfo：：bRetryInitialConnect 标志，SDK 可以被强制重新尝试连接。

## 反向连接
OPC UA 反向连接功能通过两个 UaSession 连接设置提供：SessionConnectInfo：：bIsReverseConnect 和 SessionConnectInfo：：sClientEndpointUrl。客户端端点URL指定服务器用于建立服务器与客户端之间TCP/IP连接的地址。URL中指定的端口必须在防火墙中打开。

反向连接只能通过与 UaSession：：beginConnect 的异步连接实现，因为连接建立可能需要一段时间。

反向连接功能的服务器发现通过 UaDiscovery：：startReverseDiscovery 和回调 UaReverseDiscoveryCallback 完成。

发现（UaDiscovery：：startReverseDiscovery）和连接建立（UaSession：：beginConnect）只能在同一进程中同时工作，因为这两个动作都需要客户端进程开启端口，而端口仅限于一个进程。

在一个进程中，UaSession 和 UaDiscovery 共享一个客户端端点的端口 URL。进来的反向TCP/IP连接首先用于以UaSession：：beginConnect启动的未完成连接建立。如果没有未连接连接，则使用进来的反向TCP/IP连接进行发现。

## 句柄和标识
### SessionID
SessionId是服务器内部创建的句柄，用于服务器与客户端（SDK）之间的通信。根据所使用的协议，SessionId可能与安全相关，不应在客户端或服务器应用之外可见。因此，SessionId不会在API上暴露，以确保它不会在客户端的图形界面中被错误暴露。由于会话上下文由UaSession对象管理，因此该句柄不需要在该对象之外。

### ClientConnectionId
clientConnectionId是客户端（SDK用户）定义的句柄，用于识别回调中的会话，如果同一回调对象被用于多个会话。clientConnectionId 只是 SDK 的传递，所有对该句柄的要求都由应用逻辑定义。clientConnectionId从未与服务器交换。

### TransactionID
transactionId是客户端（SDK用户）定义的句柄，用于识别回调中异步服务调用的结果。它是所有异步 beginXXX 服务调用的一个参数。transactionID 只是 SDK 的一个传递，所有对该句柄的要求都由应用逻辑定义。transactionId从未与服务器交换。它只是和客户端SDK中的事务对象一起存储。



# 订阅概述

API 文档 UaSubscription
API 文档回调 UaSubscriptionCallback


UaSubscription 类代表服务器上的单一订阅。通过UaSubscriptionCallback，使用监控项目创建的数据和事件流作为回调提供。

UaSubscription类不维护参数和MonitoredItems的本地副本。如果订阅不再有效，SDK 用户需负责在服务器上重新创建订阅。无效订阅通过UaSubscriptionCallback：：subscriptionStatusChanged报告

UaSubscription对象是线程安全的。

## 句柄和标识
### 订阅ID
SubscriptionId是服务器内部创建的一个句柄，用于服务器与客户端（SDK）之间的通信。由于 SubscriptionId 是转移订阅到另一个会话的必要条件，因此 SubscriptionID 会暴露给 SDK 用户。

根据所使用的协议，SessionId可能与安全相关，不应在客户端或服务器应用之外可见。因此，SessionId不会在API上暴露，以确保它不会在客户端的图形界面中被错误暴露。然而，SubscriptionId是转移订阅到另一个会话所必需的，且与安全无关。

### ClientSubscriptionHandle
clientSubscriptionHandle 是客户端（SDK 用户）定义的句柄，用于识别回调中的订阅，前提是同一个回调对象用于多个订阅。clientSubscriptionHandle 只是 SDK 的直通，所有对该句柄的要求都由应用逻辑定义。clientSubscriptionHandle 从不与服务器交换。

### TransactionID
transactionId是客户端（SDK用户）定义的句柄，用于识别回调中异步服务调用的结果。它是所有异步 beginXXX 服务调用的一个参数。transactionID 只是 SDK 的一个传递，所有对该句柄的要求都由应用逻辑定义。transactionId从未与服务器交换。它只是和客户端SDK中的事务对象一起存储。

### 监控项目
MonitoredItemId 是服务器定义的句柄，在 UaMonitoredItemCreateResults 中返回。必须修改或删除被监控的项目。

客户端Handle由SDK用户分配，且每个UaSubscriptionCallback实例应是唯一的。如果跨订阅分配了唯一的监控项目 ClientHandle，则 clientSubscriptionHandle 对数据变更或事件回调不具关联性。在这种情况下，clientSubscriptionHandle 只对订阅状态更改的回调相关。