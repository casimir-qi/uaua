UaSession 类管理 UA 客户端应用程序会话。

这是用于连接任何 OPC UA 服务器的主要类。该类管理与 OPC 统一体系结构服务器的连接以及与服务器建立的应用程序会话。该类将所有与订阅无关的 UA 服务作为同步和异步方法提供。

连接调用建立并维护与服务器的会话。连接成功后，SDK 会监控连接。连接状态更改通过回调 UaSessionCallback：：connectionStatusChanged 报告。

UaSession 对象是线程安全的。

UaClientSdk::ServiceSettings类包含了调用的通用设置，如超时时间、请求句柄和诊断信息。该类提供了创建和销毁ServiceSettings对象的方法，并允许配置服务调用的超时时间、请求句柄以及从服务器返回的诊断信息的位掩码，以便进行更精细的错误处理和诊断。


# UaSessionCallback
## AI总结
### Abstract

UaSessionCallback 定义了 UaSession 类的回调接口，用户需实现该接口以接收连接状态变化与异步服务调用的回调；SDK 的回调多来自工作线程，需在 GUI 主线程中同步数据，并避免在回调中执行耗时操作或在回调内调用 disconnect 以避免死锁。

### Key Points

- 回调接口用于接收连接状态变化与异步服务调用结果。
- connectionStatusChanged 提供整体连接状态变更的通知，通常在 connect/disconnect 或连接中断后触发。
- 其他诸如 readComplete、writeComplete、callComplete 等回调负责异步服务调用的结果回传。
- connectError 用于在建立连接阶段报告错误或诊断信息，支持可覆盖性（overridable）。
- 回调来自 SDK 的工作线程，应用需自行确保数据与 GUI 主线程同步。
- 调用 UaSession::disconnect() 将在断开时阻塞，且断开后不再发送回调，需等待所有异步服务完成。
- newSecurityKeySet、newServerCertificate、sslCertificateValidationFailed 等回调提供安全通道相关的事件与诊断信息。
- serverCertificateValidationRequired 与 sslCertificateValidationFailed 提供证书验证相关的回调机制，允许应用控制信任策略。
- 具体的扩展点包括 addNodesComplete、addReferencesComplete、browseComplete、browseNextComplete、translateBrowsePathsToNodeIdsComplete、registerNodesComplete、unregisterNodesComplete、deleteNodesComplete、deleteReferencesComplete、historyRead*Complete、historyUpdate*Complete、historyDelete*Complete 等，用于对应的异步操作结果回传。
- newServerCertificate 回调用于处理服务器证书变化的情形（有默认实现）。
- 您应实现所需的回调集合，仅覆盖实际使用的异步调用相关回调。

## 文档译文

详细说明
UaSessionCallback 定义了 UaSession 类的回调接口。该回调接口需要由 UaSession 类的用户实现，才能接收来自客户端 SDK 的连接状态更改回调。该接口还为异步服务调用提供回调函数。SDK 的用户只需要实现已使用的异步调用的回调。来自 SDK API 的回调来自工作线程，而不是来自用于连接到服务器的线程。如有必要，客户端 API 的用户有责任将回调提供的数据与 GUI 主线程同步。您永远不应该在回调中执行任何持久的作。明确不允许从回调中调用 UaSession::disconnect（） 并导致客户端应用程序中出现死锁。调用 UaSession::disconnect（） 后，将不再为该 UaSession 对象发送回调。断开连接将被阻止，直到所有未完成的服务调用完成并返回结果。这包括对客户端应用程序的任何异步服务调用和未完成的回调。调用 UaSession::disconnect（） 时不允许阻止回调的执行。这也将导致客户端应用程序中的死锁。
## 关键接口
### 纯虚函数 connectionStatusChanged 
virtual void UaClientSdk::UaSessionCallback::connectionStatusChanged	(	OpcUa_UInt32 	clientConnectionId,
UaClient::ServerStatus 	serverStatus 
)		
pure virtual

发送更改状态。

每当连接状态发生变化时，SDK 都会调用此回调函数。如果调用了 UaSession：：connect（） 或 UaSession：:d isconnect（），或者连接监控检测到连接中断或能够重新建立连接，则通常会出现这种情况。

如果状态未连接，则应用程序通常不需要使用 UaSession 对象执行任何作，否则不会调用服务。唯一可能需要特殊作的情况是状态 NewSessionCreated。在这些情况下，SDK 无法重复使用旧会话，并在重新连接期间创建了新会话。这需要重做新会话的注册节点或读取命名空间数组。



参数

|||||
|-|-|-|-|
|[in]|clientConnectionId|客户端连接 ID|受影响会话的客户端定义句柄|
|[in]|serverStatus|地位|新连接状态。有关更多详细信息，请参阅 [UaClient：：ServerStatus](html/classUaClientSdk_1_1UaClient.html#a3f55bd54bbf50515656f2b9ab621dc7f)。|


