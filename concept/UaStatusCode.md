#UaStatusCode
 Class Reference
## 详细说明
该类封装了 OPC UA 数据类型 StatusCodes 以及经典 OPC 代码之间的转换。

OPC UA 中的 StatusCode 是一个数值，用于报告 OPC UA 服务器执行的作的结果。此代码可能具有更详细地描述状态的相关诊断信息;但是，代码本身旨在为客户端应用程序提供足够的信息，以决定如何处理 OPC UA 服务的结果。

派生类 UaStatus 用于将 StatusCode 与关联的诊断信息组合到一个对象中。

StatusCode 是一个 32 位无符号整数。前 16 位表示代码的数值，用于检测特定错误或条件。底部 16 位是位标志，其中包含其他信息，但不会影响 StatusCode 的含义。所有 OPC UA 客户端在使用结果之前都应始终检查与结果关联的 StatusCode。应谨慎使用具有不确定/警告状态的结果，因为这些结果可能并非在所有情况下都有效。永远不要使用状态为坏/失败的结果。

如果作正常完成并且结果始终有效，则 OPC UA 服务器应返回良好/成功状态代码。不同的 StatusCode 值可以向客户端提供其他信息。

如果 OPC UA 服务器无法按照客户端请求的方式完成作，则应使用不确定/警告状态代码，但是，该作并未完全失败。

StatusCode 前 16 位的代码由 OPC UA 规范定义，不允许使用特定于应用程序的代码扩展这些代码。必须在诊断信息中提供特定于应用程序的代码。

所有 OPC UA 定义的代码都有一个符号名称和一个数值。SDK 提供定义，其中包含已定义代码的符号名称。一般定义是 OpcUa_Good 和 OpcUa_Bad 。其他更具体的好代码的所有定义都以OpcUa_Good 开头，例如 OpcUa_GoodMoreData 。其他更具体的坏代码的所有定义都以OpcUa_Bad开头，例如 OpcUa_BadInvalidArgument。

UaStatusCode::toString() 方法返回状态代码值的符号名称。

下表包含 StatusCode 位分配。
### 高位 位分配

|字段|位范围|描述|
|-|-|-|
|Severity 严厉|30:31|指示 StatusCode 是表示良好 （00）、不良 （10） 还是不确定 （01） 条件。|
|Reserved 保留|29:28|保留以备将来使用。应始终为零。|
|SubCode 子代码|16:27|代码是分配用于表示不同条件的数值。每个代码都有一个符号名称和一个数值。|
|StructureChanged 结构已更改|15:15|指示自上次通知以来关联数据值的结构已更改。客户端不应处理数据值，除非他们重新读取元数据。请参阅表后的详细说明。|
|SemanticsChanged 语义改变|14:14|指示关联数据值的语义已更改。客户端在重新读取与变量关联的元数据之前不应处理数据值。详细说明见表后。|
|Reserved 保留|12:13|保留以供将来使用。应始终为零。|
|InfoType 信息类型|10:11|信息位中包含的信息类型。有效选项为 NotUsed (00) 和 DataValue (01)。所有其他选项均保留供将来使用。|
|InfoBits 信息比特|0:9|限定 StatusCode 的附加信息位。这些位的结构取决于信息类型字段。|



下表包含 DataValue InfoBits
### 低位 位分配
|信息类型|位范围|描述|
|-|-|-|
|LimitBits 限制位|8:9  |与数据值关联的限制位。限制位的含义  无 (00)  该值可以自由改变，  低 (01)  该值处于数据源的下限，  高 (02)  该值处于数据源的上限，  常数 (11)  该值是恒定的并且不能改变。  |
|Overflow 溢出|7:7|如果设置了该位，则不会返回所有检测到的更改，因为 MonitoredItem 的服务器队列缓冲区已达到其限制并且必须清除数据。|
|Reserved 保留|5:6|保留以供将来使用。应始终为零。|
|HistorianBits 历史数据比特|0:4|这些位仅在读取历史数据时设置。它们指示数据值的来源，并提供影响客户如何使用数据值的信息。历史数据的比特有其含义  原始 (XXX00)  原始数据值，  已计算 (XXX01)  计算出的数据值，  插值 (XXX10)  插值后的数据值，  保留（XXX11），  部分 (XX1XX)  以不完整区间计算出的数据值，  额外数据 (X1XXX)  隐藏同一时间戳的其他数据的原始数据值，  多值 (1XXXX)  多个值符合聚合条件（即同一时间间隔内不同时间戳的多个最小值）|


# UaStatus
UaStatus
详细说明
此类处理状态代码、状态代码的转换和诊断信息。

该类派生自 UaStatusCode，用于将 StatusCode 与一个对象中的关联诊断信息组合在一起。类 UaStatusCode 封装了 OPC UA 数据类型 StatusCodes 以及经典 OPC 代码之间的转换

有关 StatusCode 的相关文档和函数，请参阅 UaStatusCode。
注： UaStatus 有一个成员变量 UaDiagnosticInfo * m_pDiagnosticInfo;

# 其他
uabasecpp\statuscode.h 文件

UaDiagnosticInfo 和 UaDiagnosticInfos 的关系
```C++
/** An array of UaDiagnosticInfo */
typedef UaPointerArray<UaDiagnosticInfo> UaDiagnosticInfoRawArray;

// Creates a diagnostic info array for OPC UA service calls
UABASE_EXPORT void createDiagnosticInfoArray(
    OpcUa_UInt32                    returnDiagnostics,
    const UaDiagnosticInfoRawArray& pDiagnosticInfoArray,
    UaDiagnosticInfos&              diagnosticInfos,
    OpcUa_Boolean&                  isDiagAvailable,
    OpcUa_ResponseHeader*           responseHeader);

// Creates a diagnostic info array for OPC UA service calls
// appends the string from pDiagnosticInfoArray to existingTable and creates diagnosticInfos
UABASE_EXPORT void createDiagnosticInfoArray(
    OpcUa_UInt32                    returnDiagnostics,
    std::map<UaString, OpcUa_Int32> existingTable,
    const UaDiagnosticInfoRawArray& pDiagnosticInfoArray,
    UaDiagnosticInfos&              diagnosticInfos,
    OpcUa_Boolean&                  isDiagAvailable,
    OpcUa_ResponseHeader*           responseHeader);

// Create diagnostic info - for service fault
UABASE_EXPORT void createDiagnosticInfo(
    OpcUa_UInt32            returnDiagnostics,
    UaDiagnosticInfo*       pDiagnostics,
    OpcUa_DiagnosticInfo**  ppServiceDiagnostics,
    OpcUa_Int32&            noOfStringTable,
    OpcUa_String**          ppStringTable);

// Extract a UaDiagnosticInfo from OpcUa_DiagnosticInfo and a string table
UABASE_EXPORT UaDiagnosticInfo* extractUaDiagnosticInfo(
    const OpcUa_DiagnosticInfo& diagnosticInfo,
    OpcUa_Int32                 noOfStringTable,
    OpcUa_String*               pStringTable);
分析一下这几个函数
```

好的，根据您提供的函数签名，这些函数都属于 UaStack SDK 的底层工具函数，专门用于处理 OPC UA 服务调用中的 **诊断信息 (`DiagnosticInfo`)**。

它们的核心任务是在高效传输所需的 **C 结构体 (`OpcUa_DiagnosticInfo`)** 和易于编程的 **C++ 封装类 (`UaDiagnosticInfo`)** 之间进行转换和管理。

以下是对这些函数的分析和总结：

---

## 🛠️ UaStack 诊断信息 (`DiagnosticInfo`) 处理函数分析

### 核心概念：诊断信息与字符串表

OPC UA 的诊断信息 (`DiagnosticInfo`) 结构体中不直接包含完整的错误字符串。为了提高效率，字符串（如源文件名、堆栈信息）会被放在一个单独的 **字符串表 (`StringTable`)** 中，而 `DiagnosticInfo` 只存储指向该表中字符串的 **索引**。

这些函数的主要工作就是管理这个 **字符串表** 和 **索引** 的转换。

### 1. `createDiagnosticInfoArray` (函数重载)

这两个重载函数用于 **创建发送给客户端的诊断信息数组**。它们在服务器响应时被调用。

| 函数名 | 签名差异 | 核心功能 | 角色 |
| :--- | :--- | :--- | :--- |
| **重载 1** | 接受 `responseHeader` | **从头创建** 诊断信息数组和字符串表。 | **服务器响应** |
| **重载 2** | 接受 `existingTable` | **追加到现有字符串表**。将新的诊断信息字符串添加到已存在的字符串表中，并创建相应的 `OpcUa_DiagnosticInfos` 数组。 | **服务器响应 (分段/复杂)** |

**共同输入/输出：**

* `returnDiagnostics` (`OpcUa_UInt32`): 客户端在请求中指定的诊断信息返回级别（例如，只返回 Bad 状态的诊断信息）。
* `pDiagnosticInfoArray` (`const UaDiagnosticInfoRawArray&`): 包含待处理的 **C++ 封装对象** (`UaDiagnosticInfo`) 数组。这是服务器业务逻辑产生的诊断信息。
* `diagnosticInfos` (`UaDiagnosticInfos&`): **输出** 目标，包含准备好传输的 **C 结构体** (`OpcUa_DiagnosticInfo`) 数组。

### 2. `createDiagnosticInfo`

此函数用于处理 **服务故障 (Service Fault)** 时的单个诊断信息。

| 函数签名 | 核心功能 | 角色 |
| :--- | :--- | :--- |
| `createDiagnosticInfo(...)` | **创建单个服务诊断信息。** 用于在整个服务调用失败（而非单个操作项失败）时，生成附加的诊断信息。 | **服务器故障** |

**输入/输出：**

* `pDiagnostics` (`UaDiagnosticInfo*`): 输入的单个 **C++ 诊断信息对象**。
* `ppServiceDiagnostics` (`OpcUa_DiagnosticInfo**`): **输出** 指针，指向生成的单个 **C 结构体诊断信息**，该信息将被放入服务故障消息中。
* `ppStringTable` (`OpcUa_String**`): **输出** 诊断信息关联的 **字符串表**。

### 3. `extractUaDiagnosticInfo`

此函数用于 **解析和提取接收到的诊断信息**。它在客户端接收到服务器响应后被调用。

| 函数签名 | 核心功能 | 角色 |
| :--- | :--- | :--- |
| `extractUaDiagnosticInfo(...)` | **将 C 结构体和字符串表转换为 C++ 对象。** 客户端接收到服务器响应后，需要将 `OpcUa_DiagnosticInfo` 中的索引替换为实际的字符串内容。 | **客户端解析** |

**输入/输出：**

* `diagnosticInfo` (`const OpcUa_DiagnosticInfo&`): 输入的 **C 诊断信息结构**（包含索引）。
* `pStringTable` (`OpcUa_String*`): 输入的 **字符串表数组**。
* **返回值** (`UaDiagnosticInfo*`): **输出**，返回一个新创建的 **C++ 封装对象**，其中包含实际的、可读的错误字符串。

## 总结：数据流

这些函数反映了 OPC UA 诊断信息的处理流程：

| 阶段 | 实体 | 操作 | 关键函数 |
| :--- | :--- | :--- | :--- |
| **服务器生成 (C++)** | `UaDiagnosticInfo` | 服务器业务逻辑创建详细错误信息。 | N/A (业务逻辑) |
| **服务器准备响应 (C/C++)** | `UaDiagnosticInfo` (C++) → `OpcUa_DiagnosticInfo` (C) + String Table | 将 C++ 对象转换为高效传输的 C 结构体（**索引化**）。 | `createDiagnosticInfoArray` |
| **客户端接收 (C)** | `OpcUa_DiagnosticInfo` (C) + String Table | 客户端接收索引和字符串表。 | N/A (传输层) |
| **客户端解析 (C++ 封装)** | `OpcUa_DiagnosticInfo` (C) + String Table → `UaDiagnosticInfo` (C++) | **反索引化**，将索引替换为实际字符串，生成易于使用的 C++ 对象。 | `extractUaDiagnosticInfo` |

Would you like me to discuss how `UaStatus` utilizes these diagnostic information objects?