
# 打印日志

## 服务端
使用配置开启追踪功能，在文件ServerConfig.xml 截取。
```xml
<Trace>
      <!--Enable UA stack trace true/false-->
      <UaStackTraceEnabled>true</UaStackTraceEnabled>
      <!--UA stack trace level. Possible values NONE, ERROR, WARNING, SYSTEM, INFO, DEBUG, CONTENT, ALL
                NONE    - No Trace
                ERROR   - Critical errors (unexpected and/or requiring external actions) which require attention
                WARNING - Non-critical faults which should not go unnoticed but are handled internally
                SYSTEM  - Rare major events (good cases) like initializations, shut-down, etc.
                INFO    - Regular good case events like connects, renews
                DEBUG   - Used for debugging purposes
                CONTENT - Used to add additional content (i.e. whole message bodies) to debug traces
                ALL     - All
            -->
      <UaStackTraceLevel>ALL</UaStackTraceLevel>
      <!--Enable UA server application trace true/false-->
      <UaAppTraceEnabled>true</UaAppTraceEnabled>
      <!--UA server application trace level. Possible values NoTrace, Errors, Warning, Info, InterfaceCall, CtorDtor, ProgramFlow, Data
                NoTrace       - No Trace
                Errors        - Unexpected errors
                Warning       - Unexpected behaviour that is not an error
                Info          - Information about important activities like connection establishment
                InterfaceCall - Calls to module interfaces
                CtorDtor      - Creation and destruction of objects
                ProgramFlow   - Internal program flow
                Data          - Data
            -->
      <UaAppTraceLevel>Data</UaAppTraceLevel>
      <!--Maximum number of trace entries in one file-->
      <UaAppTraceMaxEntries>100000</UaAppTraceMaxEntries>
      <!--Maximum number of backup files-->
      <UaAppTraceMaxBackup>5</UaAppTraceMaxBackup>
      <!--Do not flush the file after each trace entry. The trace file gets flushed automatically from time to time anyway.
                - For maximum trace performance you should set this option to true.
                - If you have issues with missing trace entries in case of an application crash, you should set this option to false. -->
      <UaAppTraceDisableFlush>false</UaAppTraceDisableFlush>
      <!--Trace file-->
      <UaAppTraceFile>[TracePath]/UaServerCPP.log</UaAppTraceFile>
      <!--Trace event options. Possible values Disabled, History, HistoryAndEvents-->
      <TraceEvents>History</TraceEvents>
      <!--Enable PubSub stack trace true/false-->
      <PubSubStackTraceEnabled>false</PubSubStackTraceEnabled>
    </Trace>

```

## 客户端
使用代码开启和关闭追踪功能在源代码文件 `client_cpp_sdk\client_cpp_sdk.cpp`
代码示例
```c++
#if CLIENT_CPP_SDK_ACTIVATE_TRACE
        /* Activate client trace*/
        LibT::initTrace( UaTrace::Data, 10000, 5, UaString("%1/uaclient.log").arg(szAppPath), "sample client");
        LibT::setTraceActive(true);

        /* Set Trace Settings for stack*/
        LibT::setStackTraceActive(true);
        UaPlatformLayer::changeTraceSettings(OpcUa_True, OPCUA_TRACE_OUTPUT_LEVEL_INFO);
#endif /* CLIENT_CPP_SDK_ACTIVATE_TRACE */

        mainMenu();

#if CLIENT_CPP_SDK_ACTIVATE_TRACE
        /* Close Trace */
        LibT::closeTrace();
#endif /* CLIENT_CPP_SDK_ACTIVATE_TRACE */
```

级别枚举
```c++
    /// @brief Definition of the TraceLevel Enumeration
    enum TraceLevel {
        NoTrace = 0,    /*!< Trace_Level: No Trace */
        Errors,         /*!< Trace_Level: Unexpected errors */
        Warning,        /*!< Trace_Level: Unexpected behaviour that is not an error */
        Info,           /*!< Trace_Level: Information about important activities like connection establishment */
        InterfaceCall,  /*!< Trace_Level: Calls to module interfaces */
        CtorDtor,       /*!< Trace_Level: Creation and destruction of objects */
        ProgramFlow,    /*!< Trace_Level: Internal program flow */
        Data            /*!< Trace_Level: Data */
    };
```


您提供的日志片段详细记录了 OPC UA 客户端（`UaSession`）通过 UaStack 建立连接和打开安全通道的整个过程。

这段日志是在客户端代码中设置了非常详细的追踪级别后捕获的，其中：

  * `LibT::initTrace(UaTrace::Data, 10000, 5, "uaclient.log", "sample client");` 开启了客户端 SDK 的高级追踪。
  * `UaPlatformLayer::changeTraceSettings(OpcUa_True, OPCUA_TRACE_OUTPUT_LEVEL_ALL);` 和 `LibT::setStackTraceActive(true);` 开启了 **底层 UaStack** 的所有追踪级别。

下面是日志内容的详细分析，按时间顺序分解连接步骤：

-----

## 🔬 UaStack 客户端连接日志分析

### 阶段 1: 会话初始化与配置 (Setup)

客户端创建了 `UaSession` 对象并开始连接 。

| 时间 | 动作 | 关键信息 |
| :--- | :--- | :--- |
| 02:34:58.685Z | `Ctor UaSession` | `UaSession` 对象被实例化. |
| 02:34:58.709Z | `UaSession::connect` | 尝试连接到端点 `opc.tcp://localhost:48010`. |
| 02:34:58.709Z | `SessionConnectInfo` | 客户端配置了会话参数 :<br>• `ApplicationUri`: `urn:debian:UnifiedAutomation::Client_Cpp_SDK` <br>• `SessionTimeout`: 1,200,000 ms (20 minutes) <br>• `SecureChannelLifetime`: 3,600,000 ms (1 hour) <br>• `AutomaticReconnect`: `true`  |
| 02:34:58.715Z | `SessionSecurityInfo` | **安全设置** :<br>• `MessageSecurityMode`: **None** <br>• `SecurityPolicy`: `http://opcfoundation.org/UA/SecurityPolicy#None`  |

**结论:** 客户端准备以 **不加密、无签名** 的方式连接服务器。

### 阶段 2: 传输层连接 (TCP/IP)

客户端调用底层 UaStack 函数创建和连接传输通道.

| 时间 | 动作 | 关键信息 |
| :--- | :--- | :--- |
| 02:34:58.715Z | `CALL OpcUa_Channel_Create` | UaStack 创建新的通信通道. |
| 02:34:58.716Z | `OpcUa_P_ParseUrl` | 客户端解析 URL `opc.tcp://localhost:48010`. |
| 02:34:58.716Z | `OpcUa_P_ParseUrl` | 解析主机名 `"localhost"`. |
| 02:34:58.719Z | Address Resolution | 成功将 `localhost` 解析为 IPv4 地址 `127.0.0.1` 和 IPv6 地址 `::1`. |
| 02:34:58.719Z | `OpcUa_P_SocketManager_CreateClient` | 创建新的客户端套接字 (raw socket 6). |
| 02:34:58.719Z | `NetworkThread` Start | 专门的网络线程启动，开始服务套接字事件. |
| 02:34:58.720Z | `TcpConnection_ConnectEventHandler` | **TCP 连接建立成功** 到 `127.0.0.1:48010`. |

**结论:** TCP 三次握手成功，客户端现在拥有一个到服务器的开放套接字。

### 阶段 3: OPC UA Hello/Acknowledge 握手

连接建立后，OPC UA 客户端发送 `HELLO` 消息，服务器应答 `ACKNOWLEDGE` 消息。

| 时间 | 动作 | 关键信息 |
| :--- | :--- | :--- |
| 02:34:58.720Z | `[uastack] Sending:` | 客户端发送 `HELLO` 消息 .<br>• `PV:0` (Protocol Version) <br>• `RB:65536` (Receive Buffer Size) <br>• `MMS:16777216` (Max Message Size)  |
| 02:34:58.720Z | `OpcUa_TcpStream_Flush` | 消息长度 57 字节，发送完成. |
| 02:34:58.720Z | `OpcUa_TcpStream_DataReady` | 客户端接收到 28 字节数据. |
| 02:34:58.721Z | `TcpConnection_ReadEventHandler` | 接收到的消息类型是 **ACKNOWLEDGE**. |
| 02:34:58.721Z | `Received:` / `Set:` | 确认服务器和客户端协商后的参数一致. |

**结论:** OPC UA 传输层握手成功，客户端和服务器已同意传输参数。

### 阶段 4: 打开安全通道 (OpenSecureChannel)

在传输层握手完成后，客户端开始建立安全通道。由于配置是 `SecurityPolicy#None`，这相当于发送一个未加密的通道请求。

| 时间 | 动作 | 关键信息 |
| :--- | :--- | :--- |
| 02:34:58.721Z | `OpcUa_SecureConnection_OnNotify` | Connect event received, trying to open secure channel. |
| 02:34:58.721Z | `BeginOpenSecureChannel` | 请求 Policy `None`，Mode `1` (None). |
| 02:34:58.721Z | `OpcUa_SecureStream_PrepareForSending` | 准备发送 `OpenSecureChannel` 请求消息 .<br>• `MessageType 2` (OpenSecureChannel) <br>• `SN 1` (Sequence Number 1)  |
| 02:34:58.721Z | `OpcUa_TcpStream_Flush` | 发送 `OpenSecureChannel` 请求 (消息长度 133 字节). |

**结论:** 客户端已成功发送 `OpenSecureChannel` 请求。日志片段在此处结束，下一步应是等待服务器返回 `OpenSecureChannel` 服务的响应。如果响应成功，安全通道（Secure Channel）将建立，客户端随后将开始创建 OPC UA 会话（Session）。



您提供的日志片段是 **UaStack OPC UA 服务器** 在启动和初始化阶段的详细追踪信息。它展示了服务器如何加载配置、初始化核心模块、设置版本信息和安全策略。

以下是日志内容的简要分析：

---

## 🔬 UaStack Server 启动日志分析

### 1. 服务器启动与配置加载

| 时间 | 动作 | 关键信息 |
| :--- | :--- | :--- |
| 06:31:08.652Z | `UaCoreServerApplication::start` | 服务器应用程序开始启动。 |
| 06:31:08.653Z | `ServerConfigXml::loadConfiguration` | 服务器开始加载其 XML 配置文件。 |
| **06:31:08.656Z** | **Warning: `UaEndpointXml::setXmlConfig`** | ⚠️ **警告：** 服务器的某个端点配置中启用了 `http://opcfoundation.org/UA/SecurityPolicy#None`。这意味着客户端可以**无需证书检查**即可连接。这通常是为了方便调试，但在生产环境中是一个**安全风险**。 |
| 06:31:08.656Z | `ServerConfigXml::loadConfiguration` | 配置加载完成，返回成功 (`ret=0x0`)。 |
| 06:31:08.656Z | `UaDir::mkpath` | 尝试创建必要的目录结构，但日志显示路径已存在，这不是错误。 |

### 2. 版本与环境信息

服务器初始化 **CoreModule** 时，输出了详细的软件和环境信息。

| 模块 | 版本/信息 | 编译环境 |
| :--- | :--- | :--- |
| **UA Server SDK**| `1.8.7.644` (2025年3月19日编译) | `GNU C++ 12.2` |
| **UA Stack** | `1.5.11` (版本/Git SHA) | `Unified Automation GmbH` |
| **UA Stack Platform**| 运行在 **Linux** 平台上。 | `GNU C++ 12.2` |
| **OpenSSL** | `OpenSSL 3.0.0 7 sep 2021` | 服务器正在使用 OpenSSL 进行安全功能支持。 |

### 3. 核心配置信息（静态与动态）

日志详细记录了底层协议栈（UA Stack）和平台层的配置参数：

* **线程/并发：** `Multithreaded:!0` (启用多线程)，`ThreadPoolReloopTime:500`。
* **安全：** `SecurityTokenLifeTimeMin/Max` 定义了安全令牌的生命周期（30秒到3600秒）。
* **连接限制：** `TcpListenerMaxConnections:102` (TCP 监听器最大连接数)，`HttpsListenerMaxConnections:50` (HTTPS 监听器最大连接数)。
* **安全策略支持：** 明确列出了支持的各种安全策略（如 `Basic256Sha256` 等），并显示 `SupportPolicyNone:!0` (支持无安全策略)。

### 4. 模块实例化与启动

服务器实例化了核心组件和节点管理器。

* **NodeManagerUaNode：** 实例化了 UA 命名空间 (`NsUri=http://opcfoundation.org/UA/`) 的节点管理器。
* **IOManagerUaNode：** 实例化了 IO 管理器 (ID=6)。
* **CoreTransactions：** 实例化了核心事务处理模块。
* **ServerManager::startUp：** 服务管理器开始启动，这是服务器进入运行状态的关键步骤。
* **EventManagerBase::registerEventType：** 注册了一个事件类型，表明服务器正在初始化事件系统。

### 总结

该日志片段成功捕获了 OPC UA 服务器从调用 `start` 到 **完成所有核心模块和配置初始化** 的完整过程。服务器已确认其依赖和配置，准备好进入监听和处理客户端请求的状态。 **最值得注意的是安全警告：服务器已配置为允许不安全的连接。**