读和写
# 读函数示例
L3GettingStartedClientLesson01.html
```C++
UaStatus SampleClient::read()
{
    UaStatus          result; 
    ServiceSettings   serviceSettings;
    UaReadValueIds    nodeToRead;
    UaDataValues      values;
    UaDiagnosticInfos diagnosticInfos;
 
    // Configure one node to read
    // We read the value of the ServerStatus -> CurrentTime
    nodeToRead.create(1);
    nodeToRead[0].AttributeId = OpcUa_Attributes_Value;
    nodeToRead[0].NodeId.Identifier.Numeric = OpcUaId_Server_ServerStatus_CurrentTime;
 
    printf("\nReading ...\n");
    result = m_pSession->read(
        serviceSettings,
        0,
        OpcUa_TimestampsToReturn_Both,
        nodeToRead,
        values,
        diagnosticInfos);
 
    if (result.isGood())
    {
        // Read service succeded - check status of read value
        if (OpcUa_IsGood(values[0].StatusCode))
        {
            printf("ServerStatusCurrentTime: %s\n", UaVariant(values[0].Value).toString().toUtf8());
        }
        else
        {
            printf("Read failed for item[0] with status %s\n", UaStatus(values[0].StatusCode).toString().toUtf8());
        }
    }
    else
    {
        // Service call failed
        printf("Read failed with status %s\n", result.toString().toUtf8());
    }
 
    return result;
}
```

-----

# 📄 UaStack 客户端同步读取变量 (Read Service) 文档

## 概述

OPC UA 客户端通过调用 `UaSession::read()` 服务，可以同步地从服务器地址空间中读取一个或多个节点的指定属性值、状态和时间戳。读取操作是基本的同步服务，结果会立即返回。

## 一、 核心 API 与数据结构

| 名称 | UaStack 类型 | 作用 |
| :--- | :--- | :--- |
| **Read 请求容器** | `UaReadValueIds` | 用于封装所有待读取节点的列表。 |
| **Read 响应容器** | `UaDataValues` | 服务器返回的数据值列表，每个元素包含值 (`Value`)、状态码 (`StatusCode`) 和时间戳。 |
| **服务函数** | `UaSession::read()` | 执行同步读取操作的核心函数。 |
| **请求节点** | `UaReadValueId` (在 `UaReadValueIds` 中) | 定义单个读取项的结构，包含 `NodeId` 和 `AttributeId`。 |

## 二、 读取操作的实现流程 (以 `ServerStatus.CurrentTime` 为例)

### 1\. 准备读取请求 (`UaReadValueIds`)

客户端需要构建一个或多个 `UaReadValueId` 结构，定义要读取的目标。

| 字段 | 值设置 | 作用 | 示例代码 |
| :--- | :--- | :--- | :--- |
| **容器大小** | `nodeToRead.create(1);` | 定义要读取的项数。 | `nodeToRead.create(1);` |
| **属性 ID** | `OpcUa_Attributes_Value` | **必须指定** 要读取的属性 ID。`Value` 是最常见的属性。 | `nodeToRead[0].AttributeId = OpcUa_Attributes_Value;` |
| **节点 ID** | `OpcUaId_Server_...` | 目标节点的 ID。示例中使用了标准的 Numeric ID。 | `nodeToRead[0].NodeId.Identifier.Numeric = OpcUaId_Server_ServerStatus_CurrentTime;` |

### 2\. 执行 Read 服务调用

调用 `UaSession::read()` 发送请求到服务器。

```cpp
result = m_pSession->read(
    serviceSettings, // 服务设置（如超时）
    0,               // maxAge: 0表示返回最新值
    OpcUa_TimestampsToReturn_Both, // 返回时间戳类型 (Server/Source/Both/None)
    nodeToRead,      // 输入：请求列表
    values,          // 输出：返回的数据值列表
    diagnosticInfos  // 输出：诊断信息
);
```

### 3\. 结果检查与数据解析

读取操作的检查分为两个层次：

#### A. 检查服务调用状态 (`result.isGood()`)

首先检查整个服务请求是否成功发送和接收。如果服务调用失败，则无法获取任何数据。

```cpp
if (result.isGood())
{
    // 服务通信成功，继续检查单个项的状态
}
else
{
    // 打印服务失败状态
    printf("Read failed with status %s\n", result.toString().toUtf8());
}
```

#### B. 检查单个项状态 (`values[i].StatusCode`)

如果服务调用成功，必须检查 **`UaDataValues`** 数组中每个返回项的 **`StatusCode`**。状态码指示了该特定读取操作是否成功。

```cpp
if (OpcUa_IsGood(values[0].StatusCode))
{
    // 数据良好：提取并解析 UaVariant
    printf("... CurrentTime: %s\n", UaVariant(values[0].Value).toString().toUtf8());
}
else
{
    // 该项读取失败
    printf("Read failed for item[0] with status %s\n", UaStatus(values[0].StatusCode).toString().toUtf8());
}
```

## 三、 数据解析：从 `UaDataValue` 中提取

服务器返回的数据存储在 `values` 数组中，每个元素是一个 `UaDataValue` 结构。

  * **`values[i].Value`：** 包含实际的数据内容，类型为底层 `OpcUa_Variant`。
  * **`UaVariant(values[i].Value).toString()`：** 在 UaStack C++ SDK 中，通常使用 `UaVariant` 构造函数来封装底层 `OpcUa_Variant`，并使用其方法（如 `toString()`）来安全地提取和格式化数据。

-----
# 相关类
### UaStatus 

### ServiceSettings 
服务设置，例如超时

### UaReadValueIds
UA 堆栈结构OpcUa_ReadValueId的数组类。
此类封装原生OpcUa_ReadValueId结构的数组，并为你处理内存分配和清理。

### UaDataValues
UA 堆栈结构 OpcUa_DataValue 的数组类。
此类封装原生 OpcUa_DataValue 结构的数组，并为你处理内存分配和清理。
#### OpcUa_DataValue
UA 堆栈结构 OpcUa_DataValue 的包装类。
此类封装原生OpcUa_DataValue结构，并为你处理内存分配和清理。UaDataValue 使用隐式共享来避免不必要的复制并提高性能。仅当您修改共享 DataValue 时，它才会为该 DataValue 创建一个副本（写时复制）。因此，分配另一个 UaDataValue 或将其作为参数传递需要恒定时间几乎与分配指针一样快。

### UaDiagnosticInfos
用于管理 DiagnosticInfo 结构数组的类。


根据示例提供的 UaStack 客户端读取（`read`）变量的示例代码，撰写一份清晰的文档，总结 OPC UA 客户端如何使用 `UaSession::read()` 函数执行同步数据读取操作。



## 文档译文：
从 OPC UA 服务器同步读取属性值。

读取服务用于读取一个或多个节点的一个或多个属性。它还允许读取数组值的子集或单个元素，并定义要返回的值的有效期限，以减少设备读取的需要。与大多数其他服务一样，读取针对批量读取操作进行了优化，而不是针对读取单个属性值进行了优化。通常所有节点属性都是可读的。对于值属性，读取权限由变量的 AccessLevel 和 UserAccessLevel 属性指示。
### 参数

||||
|-|-|-|
|[输入、输出]|服务设置  serviceSettings|一般服务设置如超时。有关更多详细信息，请参阅[ServiceSettings](html/classUaClientSdk_1_1ServiceSettings.html)。|
|[输入]|最大年龄  maxAge|要读取的值的最长期限（以毫秒为单位）。此参数允许客户端通过允许服务器返回不早于定义时间段的缓存值来减少服务器和数据源之间的通信。设置值 0 会强制服务器获取当前值。这类似于经典 OPC 中读取的设备。|
|[输入]|时间戳  timeStamps|OPC UA 定义了两个时间戳：源时间戳和服务器时间戳。此参数允许客户端定义服务器应随值返回哪些时间戳。      源时间戳仅适用于值属性。源时间戳用于反映数据源应用于变量值的时间戳。它应该指示值或状态代码的最后一次更改。源时间戳必须始终由同一物理时钟生成。为 OPC UA 添加了此时间戳类型，以涵盖获取上次值更改的时间戳的用例，该时间戳与服务器时间戳定义不同。      服务器时间戳用于反映服务器收到变量值或在异常报告更改且与数据源的连接正在运行时知道变量值准确的时间。这是经典 OPC 所期望的行为。      可能的值为：  -   OpcUa_TimestampsToReturn_Source -   OpcUa_TimestampsToReturn_Server -   OpcUa_TimestampsToReturn_Both|
|[输入]|待读节点  nodesToRead|要读取的节点和属性列表，由 [OpcUa_ReadValueId](html/structOpcUa__ReadValueId.html) 结构数组标识。  ReadValueId 包括  -   NodeId：要读取的节点的NodeId -   AttributeId：要读取的属性的 ID，例如 OpcUa_Attributes_Value -   IndexRange：请求数组或矩阵值子集的选项  IndexRange 参数用于选择数组的单个元素，或基于称为 NumericRange 的字符串语法的数组的单个索引范围。第一个元素由索引 0（零）标识。 使用单个整数选择单个元素，例如“6”。单个范围由用冒号（':'）字符分隔的两个整数表示，例如“5:7”。 多维数组可以通过指定由“,”分隔的每个维度的范围来索引。例如，可以选择 4x4 矩阵中的 2x2 块，范围为“1:2,0:1”。可以通过指定单个数字而不是范围来选择多维数组中的单个元素。例如，“1,1”指定选择二维数组中的[1,1]元素。  The following BNF describes the syntax 以下 BNF 描述了语法 <numeric-range> ::= <dimension> [',' <dimension>]|
|[输出]|值  values|[OpcUa_DataValue](/html/structOpcUa__DataValue.html) 结构数组中包含的读取结果列表。  数据值包括  -   Value：读取节点和属性组合的值 -   StatusCode：该参数用于指示该值生成的条件，从而可以作为该值可用性的指标。 -   SourceTimestamp：反映数据源应用于值的 UTC 时间戳。它仅适用于值属性。 -   ServerTimestamp：反映服务器收到该值或知道该值准确的时间。|
|[输出]|诊断信息  diagnosticInfos|诊断信息列表   诊断信息中索引的字符串表通过 serviceSettings in/out 参数提供。此参数中的字符串表始终为空，因为它已包含在 serviceSettings 中。|


# 写函数示例
  L3GettingStartedClientLesson03.html
```C++
UaStatus SampleClient::write()
{
    UaStatus            result;
    ServiceSettings     serviceSettings;
    UaWriteValues       nodesToWrite;
    UaStatusCodeArray   results;
    UaDiagnosticInfos   diagnosticInfos;
 
    // write all nodes from the configuration
    UaNodeIdArray nodes = m_pConfiguration->getNodesToWrite();
    UaVariantArray values = m_pConfiguration->getWriteValues();
    nodesToWrite.create(nodes.length());
 
    for (OpcUa_UInt32 i = 0; i < nodes.length(); i++)
    {
        nodesToWrite[i].AttributeId = OpcUa_Attributes_Value;
        OpcUa_NodeId_CopyTo(&nodes[i], &nodesToWrite[i].NodeId);
        // set value to write
        OpcUa_Variant_CopyTo(&values[i], &nodesToWrite[i].Value.Value);
    }
 
    printf("\nWriting...\n");
    result = m_pSession->write(
        serviceSettings,
        nodesToWrite,
        results,
        diagnosticInfos);
 
    if (result.isGood())
    {
        // Write service succeded - check individual status codes
        for (OpcUa_UInt32 i = 0; i < results.length(); i++)
        {
            if (OpcUa_IsGood(results[i]))
            {
                printf("Write succeeded for item[%d]\n", i);
            }
            else
            {
                printf("Write failed for item[%d] with status %s\n", i, UaStatus(results[i]).toString().toUtf8());
            }
        }
    }
    else
    {
        // Service call failed
        printf("Write failed with status %s\n", result.toString().toUtf8());
    }
 
    return result;
}
```

# 📄 UaStack 客户端同步写入变量 (Write Service) 文档

## 概述

OPC UA 客户端通过调用 **`UaSession::write()`** 服务，可以同步地向服务器地址空间中的一个或多个节点写入指定属性（通常是 `Value`）的新值。写入操作是基本的同步服务，用于修改服务器变量的状态或数据。

## 一、 核心 API 与数据结构

| 名称 | UaStack 类型 | 作用 |
| :--- | :--- | :--- |
| **Write 请求容器** | `UaWriteValues` | 用于封装所有待写入节点及其新值的列表。 |
| **Write 响应容器** | `UaStatusCodeArray` | 服务器返回的 **状态码数组**，指示每个写入操作的结果。 |
| **服务函数** | `UaSession::write()` | 执行同步写入操作的核心函数。 |
| **请求节点** | `UaWriteValue` (在 `UaWriteValues` 中) | 定义单个写入项的结构，包含目标 **NodeId**、**AttributeId** 和要写入的 **Value**。 |

## 二、 写入操作的实现流程

客户端实现写入操作主要分为三个步骤：准备请求数据、执行服务调用和处理结果。

### 1\. 准备写入请求 (`UaWriteValues`)

客户端需要构建一个或多个 `UaWriteValue` 结构，定义要写入的目标和新值。

| 字段 | 值设置 | 作用 | 示例代码中涉及的操作 |
| :--- | :--- | :--- | :--- |
| **容器大小** | `nodesToWrite.create(N);` | 定义要写入的项数。 | `nodesToWrite.create(nodes.length());` |
| **属性 ID** | `OpcUa_Attributes_Value` | **必须指定** 要写入的属性 ID。 | `nodesToWrite[i].AttributeId = OpcUa_Attributes_Value;` |
| **节点 ID** | `OpcUa_NodeId_CopyTo(...)` | 目标节点的 ID。 | `OpcUa_NodeId_CopyTo(&nodes[i], &nodesToWrite[i].NodeId);` |
| **写入值** | `OpcUa_Variant_CopyTo(...)` | 要写入的新值，封装在 `UaVariant` 中。 | `OpcUa_Variant_CopyTo(&values[i], &nodesToWrite[i].Value.Value);` |

> **注意：** 示例中使用了底层的 `OpcUa_NodeId_CopyTo` 和 `OpcUa_Variant_CopyTo` 函数来处理 C 结构体和 C++ 封装类型之间的数据复制，以确保类型安全和内存正确处理。

### 2\. 执行 Write 服务调用

调用 `UaSession::write()` 发送请求到服务器。

```cpp
result = m_pSession->write(
    serviceSettings, // 服务设置（如超时）
    nodesToWrite,    // 输入：请求列表
    results,         // 输出：返回的状态码数组
    diagnosticInfos  // 输出：诊断信息
);
```

### 3\. 结果检查与状态处理

读取操作的检查分为两个层次，与 Read 服务相似：

#### A. 检查服务调用状态 (`result.isGood()`)

首先检查整个服务请求的通信状态。如果 `result.isBad()`，则表示服务请求未到达服务器或通信失败。

```cpp
if (result.isGood())
{
    // 服务通信成功，继续检查单个项的状态
}
else
{
    // 打印服务失败状态
    printf("Write failed with status %s\n", result.toString().toUtf8());
}
```

#### B. 检查单个项状态 (`results[i]`)

如果服务调用成功，必须检查 **`UaStatusCodeArray`** (`results`) 中每个状态码。这个数组的顺序与请求数组 (`nodesToWrite`) 的顺序严格对应。

```cpp
// 遍历返回的状态码数组
for (OpcUa_UInt32 i = 0; i < results.length(); i++)
{
    if (OpcUa_IsGood(results[i]))
    {
        // 状态码良好，写入成功
        printf("Write succeeded for item[%d]\n", i);
    }
    else
    {
        // 状态码不良好，写入失败（例如，权限不足或数据类型不匹配）
        printf("Write failed for item[%d] with status %s\n", i, UaStatus(results[i]).toString().toUtf8());
    }
}
```

# 写函数相关类
### UaWriteValues
UA 堆栈结构OpcUa_WriteValue的数组类。
此类封装原生 OpcUa_WriteValue 结构的数组，并为你处理内存分配和清理。
### OpcUa_WriteValue
定义为 Write 服务的 nodesToWrite 参数类型的结构。

UaStatusCodeArray   results;
UaDiagnosticInfos   diagnosticInfos;

### OpcUa_WriteValue::AttributeId
属性的 ID。
这应是有效的属性 ID。
属性的 IntegerId 在 NodeAttributesMask 中定义。
此参数用于标识数组的单个元素或数组的单个索引范围。

### OpcUa_WriteValue::IndexRange
第一个元素由索引 0（零）标识。
如果指定的属性不是数组，则不使用此参数。但是，如果指定的属性是数组并且未使用此参数，则所有元素都将包含在范围内。如果不使用该参数则为空。

### OpcUa_WriteValue::Value
节点的属性值。
如果指定了indexRange参数，即使只写入一个元素，Value也应该是一个数组。
如果指定了 SourceTimestamp 或 ServerTimestamp，则服务器应使用这些值。如果服务器不支持写入时间戳，则返回 Bad_WriteNotSupported 错误。
如果写入值的数据类型与属性的 DataType 不是同一类型或子类型，则服务器应返回 Bad_TypeMismatch 错误。基于数据类型层次结构，服务器应接受属性数据类型的子类型。对于 value 属性，DataType 是通过 DataType 属性定义的。 ByteString 在结构上与 Byte 的一维数组相同。如果需要字节数组，服务器应接受字节字符串。
如果服务器不支持写入传递的数据编码，则返回 Bad_DataEncodingUnsupported 错误。

好的，根据您提供的 UaStack 客户端写入（`write`）变量的示例代码，我将为您撰写一份清晰的文档，总结 OPC UA 客户端如何使用 `UaSession::write()` 函数执行同步数据写入操作。

-----



好的，根据您提供的 UaStack `UaSession::read()` 和 `UaSession::write()` 示例，以下是一份关于 `UaSession` 同步读写操作的简洁总结。

---

# ✍️ UaSession 同步读写操作总结

`UaSession` 类是 UaStack 客户端执行 OPC UA 服务调用的核心接口。**读 (`read`) 和 写 (`write`)** 服务是客户端与服务器进行实时数据交换的基础同步操作。

## 1. 读操作 (`UaSession::read`)

**功能：** 从服务器读取一个或多个节点的指定属性值、状态和时间戳。

| 方面 | 关键输入结构 | 关键输出结构 | 核心要点 |
| :--- | :--- | :--- | :--- |
| **请求** | `UaReadValueIds` | `UaDataValues` | **必须指定 `NodeId` 和 `AttributeId`**（通常是 `OpcUa_Attributes_Value`）。 |
| **响应** | **无** | `UaDataValues` (数组) | 结果包含 **值 (`Value`)**、**状态码 (`StatusCode`)** 和时间戳。 |
| **检查** | `result.isGood()` | `values[i].StatusCode` | 需两层检查：先检查服务调用状态，再检查每个返回项的状态码。 |

## 2. 写操作 (`UaSession::write`)

**功能：** 向服务器写入一个或多个节点的指定属性的新值。

| 方面 | 关键输入结构 | 关键输出结构 | 核心要点 |
| :--- | :--- | :--- | :--- |
| **请求** | `UaWriteValues` | `UaStatusCodeArray` | 请求中必须包含 **`NodeId`**、**`AttributeId`** 和 **待写入的 `UaVariant` 值**。 |
| **响应** | **无** | `UaStatusCodeArray` | 返回一个状态码数组，其顺序与请求项的顺序严格对应。 |
| **检查** | `result.isGood()` | `results[i]` | 需两层检查：先检查服务调用状态，再检查每个写入项的状态码。 |

## 总结：读写操作的共同点

* **同步性：** 两个操作都是同步阻塞的，客户端等待服务器返回结果。
* **粒度：** 都可以批量处理多个节点（或属性）的请求。
* **结果处理：** 都需要进行两级错误检查：首先检查整个服务调用的通信状态，然后遍历结果数组，检查每个单独操作的状态码。

---



Wrapper class for the UA stack structure OpcUa_Variant.

Helper class for handling the OPC UA built-in data type Variant. It provides conversion and comparison functions and handles memory allocation and cleanup.

The UaVariant class acts like a union for all OPC UA built-in data types.

好的，根据您提供的 `UaVariant` 相关的文档内容，我为您总结一份详细的中文技术文档。

---

# 📄 扩展 UaVariant（变体）数据类型总结文档

## 概述

**`UaVariant`** 是 OPC UA SDK (如 UaStack) 中用于处理 **`OpcUa_Variant`** 核心结构的封装类。它实现了 OPC UA 内置数据类型 **`Variant`** 的全部功能，充当所有内置数据类型的联合体，是 OPC UA 灵活性的基础。

`UaVariant` 的主要目标是简化 C++ 编程中对 OPC UA 数据的处理，它负责内存的分配和清理，并提供类型转换和比较等辅助功能。

## 一、 `OpcUa_Variant` 的核心特性

`OpcUa_Variant`（底层结构）是一个高度灵活的容器，具有以下核心特性：

### 1. 数据的通用性（联合体）

* **所有内置数据类型的联合：** `Variant` 就像一个可以容纳所有 OPC UA 内置数据类型的“通用盒子”。这意味着它可以存储任何基本数据类型（如整数、字符串、日期时间等）的值。
* **支持数组：** `Variant` 不仅可以存储单个标量值，还可以存储 **任何内置数据类型的数组**。
* **支持复杂类型：** 它还包括对 **`OpcUa_ExtensionObject`** 的支持，这是 OPC UA 中封装自定义复杂结构体的方式。

### 2. 空值处理 (Null Value)

* **空变体：** `Variant` 可以为空（Empty）。一个空的 `Variant` 被描述为具有 **空值 (null value)**，应该像 SQL 数据库中的空列一样对待。
* **与数据类型 Null 的区别：** 理论上，`Variant` 的空值与某些数据类型（如 `String`）自身支持的空值是不同的。
* **应用层处理：** **应用程序不应该依赖于这两者之间的区别**，因为某些开发平台可能无法保留这种细微的差异。

### 3. 不支持的类型和限制

出于协议设计和语义清晰的考虑，`Variant` **不能** 直接包含以下类型的实例：

* **`DataValue` (数据值)：** `DataValue` 只有在作为服务响应的一部分并附带了 `StatusCode` 时才有意义。
* **`DiagnosticInfo` (诊断信息)：** 同样，它需要与响应状态码关联才有意义。

### 4. 嵌套限制

* **可变数组嵌套：** `Variant` 可以包含 **`Variant` 类型的数组**，但不能直接包含另一个 `Variant`。
* **深度限制：** `ExtensionObject` 和 `Variant` 都允许无限嵌套。为了防止栈溢出，**解码器（Decoder）必须支持至少 100 层的嵌套深度**。如果嵌套层级超出支持范围，解码器应报错。

## 二、 `Variant` 与 `BaseDataType` 变量的映射

在 OPC UA 地址空间中，将 `Variant` 赋值给一个变量时，必须遵守变量自身的类型限制：

* 如果一个变量的 `DataType` 被设置为 `BaseDataType`（即允许任意类型），它的值被映射到 `Variant`。
* 但是，该变量的 **`ValueRank` (值级别)** 和 **`ArrayDimensions` (数组维度)** 属性会对 `Variant` 中允许的内容施加限制。
    * **例如：** 如果变量的 `ValueRank` 是 **`Scalar` (标量)**，则 `Variant` 中只能包含单个（非数组）标量值。

## 总结

`UaVariant` 是 OPC UA 客户端开发中处理动态数据类型的核心工具。它提供了处理几乎所有内置数据类型的灵活性，但在使用时，必须注意其空值处理的约定以及对 `DataValue` 和 `DiagnosticInfo` 类型的限制。

\include-ua\uastack\opcua_builtintypes.h
# 扩展 OpcUa_NodeId_CopyTo

`OPCUA_EXPORT OPCUA_P_WARN_UNUSED_RESULT OpcUa_StatusCode OpcUa_NodeId_CopyTo(const OpcUa_NodeId* pSource, OpcUa_NodeId* pDestination);`


## OpcUa_Variant_CopyTo
OPCUA_EXPORT OPCUA_P_WARN_UNUSED_RESULT OpcUa_StatusCode OpcUa_Variant_CopyTo(const OpcUa_Variant* pSource, OpcUa_Variant* pDestination);

没有文档，根据函数名称和参数猜测是拷贝函数
---

