
## read

UaStatus UaSession::read	(	ServiceSettings & 	serviceSettings,
OpcUa_Double 	maxAge,
OpcUa_TimestampsToReturn 	timeStamps,
const UaReadValueIds & 	nodesToRead,
UaDataValues & 	values,
UaDiagnosticInfos & 	diagnosticInfos 
)	

从 OPC UA 服务器同步读取属性值。

读取服务用于读取一个或多个节点的一个或多个属性。它还允许读取数组值的子集或单个元素，并定义要返回的有效值范围，以减少设备读取次数。与其他大多数服务一样，读取服务针对批量读取操作进行了优化，而非针对读取单个属性值。通常，所有节点属性都是可读的。对于值属性，读取权限由变量的 AccessLevel 和 UserAccessLevel 属性指示。
||||
|-|-|-|
|[in,out]  |serviceSettings|常规服务设置，例如超时。有关更多详细信息，请参阅 ServiceSettings。|
|[in]|maxAge|要读取的值的最大保留时间（以毫秒为单位）。此参数允许客户端通过允许服务器返回不早于指定时间段的缓存值来减少服务器和数据源之间的通信。将值设置为 0 将强制服务器获取当前值。这类似于经典 OPC 中的设备读取。|
|[in]|timeStamps|OPC UA 定义了两个时间戳：源时间戳和服务器时间戳。此参数允许客户端定义服务器应随值返回哪些时间戳。|
|[in]|nodesToRead	|要读取的节点和属性列表，由 OpcUa_ReadValueId 结构数组标识。|
|[out]|diagnosticInfos|诊断信息列表。 诊断信息索引的字符串表通过 serviceSettings 的 in/out 参数提供。此参数中的字符串表始终为空，因为它已包含在 serviceSettings 中。|

### 参数三： timeStamps
源时间戳仅适用于值属性。源时间戳用于反映数据源应用于变量值的时间戳。它应指示值或状态代码的最后更改时间。源时间戳必须始终由同一物理时钟生成。此时间戳类型是为 OPC UA 添加的，用于满足获取上次值更改时间戳（与服务器时间戳定义不同）的使用场景。

服务器时间戳用于反映服务器接收到变量值的时间，或者在异常报告更改且与数据源的连接正常运行的情况下，服务器确认该值准确的时间。这是经典 OPC 的预期行为。

可选值包括：

OpcUa_TimestampsToReturn_Source

OpcUa_TimestampsToReturn_Server

OpcUa_TimestampsToReturn_Both


### OpcUa_ReadValueId 详解
参数 nodesToRead
由 OpcUa_ReadValueId 结构数组标识。

NodeId：要读取的节点的 NodeId

AttributeId：要读取的属性的 ID，例如 OpcUa_Attributes_Value

IndexRange：用于请求数组或矩阵值的子集的选项

IndexRange 参数用于选择数组中的单个元素，或基于称为 NumericRange 的字符串语法选择数组的单个索引范围。第一个元素由索引 0（零）标识。

单个元素可以用单个整数选择，例如“6”。单个范围由两个用冒号 (':') 分隔的整数表示，例如“5:7”。

可以通过为每个维度指定一个范围并用逗号 (',') 分隔来索引多维数组。例如，可以使用范围“1:2,0:1”选择 4x4 矩阵中的一个 2x2 块。多维数组中的单个元素也可以通过指定单个数字而不是范围来选择。例如，“1,1”表示选择二维数组中的 [1,1] 元素。

以下 BNF 描述了语法：
```xml
<numeric-range> ::= <dimension> [',' <dimension>]

<dimension> ::= <index> [':' <index>]

<index> ::= <digit> [<digit>]

<digit> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
```
DataValue 由以下部分组成：

Value：读取节点及其属性组合的值

StatusCode：此参数用于指示生成该值的条件，因此可以用作该值可用性的指标。

SourceTimestamp：反映数据源应用于该值的 UTC 时间戳。它仅适用于值属性。

ServerTimestamp：反映服务器接收到该值或确认其准确的时间。


## write
UaStatus UaSession::write	(	ServiceSettings & 	serviceSettings,
const UaWriteValues & 	nodesToWrite,
UaStatusCodeArray & 	results,
UaDiagnosticInfos & 	diagnosticInfos 
)	

将变量值同步写入 OPC UA 服务器。
### 参数
||||
|-|-|-|
|[in,out]|serviceSettings  |  serviceSettings 常规服务设置，例如超时。有关更多详细信息，请参阅 ServiceSettings。|
|[in]|nodesToWrite|要写入的节点、属性和值的列表，由 OpcUa_WriteValue 结构数组标识。 WriteValue 包含：  - NodeId：要写入的节点的 NodeId - AttributeId：要写入的属性的 ID，例如 OpcUa_Attributes_Value - IndexRange：用于写入数组或矩阵值的子集的选项 - Value：要写入的值  IndexRange 参数用于选择数组中的单个元素，或基于称为 NumericRange 的字符串语法选择数组的单个索引范围。第一个元素由索引 0（零）标识。   使用单个整数选择单个元素，例如“6”。使用冒号 (':') 分隔的两个整数表示单个范围，例如“5:7”。   可以通过为每个维度指定一个范围（以逗号分隔）来对多维数组进行索引。例如，可以使用范围“1:2,0:1”来选择 4x4 矩阵中的一个 2x2 块。也可以通过指定单个数字而不是范围来选择多维数组中的单个元素。例如，“1,1”表示选择二维数组中的 [1,1] 元素。  Value 参数中数组的大小必须与 IndexRange 指定的大小匹配。 |
|[out]|results|写入结果列表|
|[out]|diagnosticInfos|诊断信息列表 诊断信息索引的字符串表通过 serviceSettings 的输入/输出参数提供。此参数中的字符串表始终为空，因为它已包含在 serviceSettings 中。|

# BNF（巴科斯范式） 语法
## UaNumericRange
用于处理 NumericRange 的帮助程序类。

数字范围提供了一种识别单维或多维数据数组的子集的方法。数字范围以字符串的形式提供，其中包含不同构造的定义语法，例如数组中的单个索引、数组的范围或多维数组的子集。
OPC UA 中 NumericRange（数值范围） 的定义及其使用的 BNF（巴科斯范式） 语法。这在处理 OPC UA 变量的数组或多维数组数据时非常重要
```
<numeric-range> ::= <dimension> [',' <dimension>]
<dimension> ::= <index> [':' <index>]
<index> ::= <digit> [<digit>]
<digit> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'  |

<numeric-range> ：：= 维度 ['，' 维度]
维度 ：：= 索引 ['：' 索引]
索引 ：：= 数字 [数字]
数字 ：：= '0' |'1' |'2' |'3' |'4' |'5' |'6' |'7' |'8' |'9'
 ```

# gemin


好的，您问到了 OPC UA 中 **`NumericRange`（数值范围）** 的定义及其使用的 **BNF（巴科斯范式）** 语法。这在处理 OPC UA 变量的数组或多维数组数据时非常重要。

我会为您解释这个概念，并拆解 BNF 规则，让您理解如何根据这个语法来定义一个数值范围字符串。

-----

## 🔢 `NumericRange`（数值范围）概念解释

### 核心功能

`NumericRange` 提供了一种机制，允许 OPC UA 客户端在读取、写入或订阅一个 **数组类型** 的变量时，**只指定和操作该数组中的一个子集（子数组）**，而无需传输整个数组。

  * **数据来源：** 它可以用于一维数组，也可以用于多维数组。
  * **格式：** 数值范围是以 **字符串** 的形式提供的，该字符串必须遵循严格的 BNF 语法。

-----

## 📝 `NumericRange` 的 BNF 语法拆解

BNF（巴科斯范式）是一种用于描述编程语言或数据格式语法的符号集。让我们逐一解析您提供的规则：

### 1\. 最小单位：`digit` (数字)

```bnf
digit ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
```

  * **含义：** `digit` 定义了一个可以使用的单个字符，即任何阿拉伯数字 `0` 到 `9`。

### 2\. 索引：`index`

```bnf
index ::= digit [digit]
```

  * **含义：** `index` 定义了数组中的一个位置（索引值）。
  * **规则：** 它由至少一个 `digit` 组成，后面可以跟 **零个或多个** `digit`。
  * **实际上：** 这就是定义一个 **非负整数**。
      * **有效索引示例：** `0`, `9`, `123`, `4567`。

### 3\. 维度：`dimension` (范围)

```bnf
dimension ::= index [':' index]
```

  * **含义：** `dimension` 定义了 **某一维度** 的数组子集。
  * **规则：**
      * 它必须包含一个 `index` (起始或单个位置)。
      * 后面可以跟 **零个或一个** `':' index` 部分。
  * **组合形式：**
    1.  **单个索引：** `index` (例如：`5`)。表示数组的 **第 5 个元素**。
    2.  **范围：** `index : index` (例如：`10:20`)。表示从 **第 10 个元素到第 20 个元素**（包含两者）。
    3.  **开放范围：** 虽然 BNF 没有明确写出，但规范通常允许省略一侧，例如 `:20`（从 0 到 20）或 `10:`（从 10 到末尾）。**根据这个简化的 BNF，您需要提供两个索引来定义范围**。

### 4\. 数值范围：`<numeric-range>` (最终字符串)

```bnf
<numeric-range> ::= dimension [',' dimension]
```

  * **含义：** 这是最终的 `NumericRange` 字符串的完整语法。
  * **规则：**
      * 它必须包含一个 `dimension`。
      * 后面可以跟 **零个或多个** `',' dimension` 部分。
  * **关键点：** 逗号 `,` 用于分隔 **多维数组** 的不同维度。

-----

## 💡 实际应用示例 (多维数组)

假设您有一个二维数组变量，其大小为 `[10, 5]`（10 行，5 列）。您需要同时指定行维度和列维度。

| 示例 `NumericRange` 字符串 | BNF 构成 | 含义 |
| :--- | :--- | :--- |
| **`5`** | `dimension` | **一维数组：** 仅访问数组的第 5 个元素。 |
| **`10:20`** | `index : index` | **一维数组：** 访问从索引 10 到索引 20 的子数组。 |
| **`1:3, 0`** | `dimension , dimension` | **二维数组：** 访问第 1 行到第 3 行，以及第 0 列（即所有行中的第 0 列数据）。 |
| **`3, 1:4`** | `dimension , dimension` | **二维数组：** 访问第 3 行，以及第 1 列到第 4 列的子数组。 |

**总结：**

您需要将 `NumericRange` 字符串理解为一种**基于逗号分隔的列表**。列表中每个元素（`dimension`）定义了该数组维度上的索引范围。对于一维数组，只有一个 `dimension`；对于多维数组，则有多个 `dimension` 用逗号分隔。




