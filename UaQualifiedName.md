好的，根据您提供的 UaStack C 结构体定义，我将为您撰写一份关于 **`OpcUa_QualifiedName`（限定名称）** 的技术文档。

-----

# 📄 `OpcUa_QualifiedName`（限定名称）技术文档

## 概述

`OpcUa_QualifiedName` 是 OPC UA 协议中的一个基础数据类型，用于在整个地址空间内**唯一地**标识一个名称。它主要由两部分组成：一个 **命名空间索引** 和一个 **文本名称**。

限定名称最常见的用途是作为节点的 **`BrowseName`**（浏览名称），用于在地址空间中进行导航和查询。

## 结构定义 (`OpcUa_QualifiedName`)

在 UaStack SDK 的底层 C 结构中，`OpcUa_QualifiedName` 定义如下：

```c
typedef struct _OpcUa_QualifiedName
{
    OpcUa_UInt16 NamespaceIndex; // 命名空间索引
    OpcUa_UInt16 Reserved;       // 保留字段 (未在文档中说明用途)
    OpcUa_String Name;           // 名称的文本部分
}
OpcUa_QualifiedName;
```

-----

## 字段详解

### 1\. `NamespaceIndex` (命名空间索引)

| 字段 | 类型 | 描述 |
| :--- | :--- | :--- |
| `NamespaceIndex` | `OpcUa_UInt16` | 一个 **16 位无符号整数**，用于标识定义该名称的命名空间。 |

  * **作用：** 命名空间机制是 OPC UA 用来避免名称冲突的关键。每个 OPC UA Server 都维护一个 **命名空间数组 (NamespaceArray)**。
  * **客户端操作：** 客户端可以通过读取服务器的 `NamespaceArray` 变量来获取该索引对应的 **URI 字符串**。
      * 索引 `0` 通常保留给 **OPC UA 规范** 定义的标准名称（`http://opcfoundation.org/UA/`）。
      * 其他索引（如 `1, 2, 3...`）通常由服务器或特定设备制造商定义。

### 2\. `Name` (名称)

| 字段 | 类型 | 描述 |
| :--- | :--- | :--- |
| `Name` | `OpcUa_String` | 名称的 **文本部分**。 |

  * **作用：** 这是人类可读的名称，例如 `Temperature`、`MotorState` 或 `Message`。
  * **限制：** 文档指出名称的文本部分最大限制为 **512 个字符**。
  * **唯一性：** 在给定的 `NamespaceIndex` 内，`Name` 字段必须是唯一的。

### 3\. `Reserved` (保留字段)

| 字段 | 类型 | 描述 |
| :--- | :--- | :--- |
| `Reserved` | `OpcUa_UInt16` | 一个保留字段，通常不应由客户端直接使用或修改。 |

-----

## 应用场景

`OpcUa_QualifiedName` 在 OPC UA 架构中广泛使用，尤其是在以下场景：

### 1\. 浏览名称 (`BrowseName`)

  * `QualifiedName` 是节点的 **`BrowseName`** 属性的类型。这是在浏览 OPC UA 地址空间时，用于识别节点的主要名称。
  * `BrowseName` 确保了名称在其命名空间内是唯一的，即使两个不同命名空间中的节点具有相同的文本名称，它们也通过不同的 `NamespaceIndex` 进行区分。

### 2\. 事件字段选择

  * 在构建事件过滤器（`UaEventFilter`）的 **选择子句 (`SelectClauses`)** 时，客户端使用 `QualifiedName` 来精确指定要从事件中检索的字段名称及其命名空间。

### 3\. 类型定义

  * 在定义结构体、变量类型或其他复杂类型时，`QualifiedName` 用于为类型分配一个唯一的名称。

-----

## 示例（C++ 封装类 `UaQualifiedName`）

虽然您提供的是底层 C 结构，但在使用 UaStack SDK 进行 C++ 开发时，通常会使用其封装类 **`UaQualifiedName`** 进行操作，以提供更友好的接口：

```cpp
// 订阅标准事件字段 "Message"，命名空间索引为 0
UaQualifiedName qNameMessage("Message", 0); 

// 订阅自定义变量 "Temperature"，命名空间索引为 3
UaQualifiedName qNameTemp("Temperature", 3);
```

#   那么`QualifiedName` 与 `NodeId`（节点 ID）之间的主要区别？

  那么`QualifiedName` 与 `NodeId`（节点 ID）之间的主要区别？

  这是一个很好的问题，因为 `QualifiedName` 和 `NodeId` 是 OPC UA 地址空间中用来标识对象的两个最基本、也最常混淆的概念。

虽然它们都用于标识 OPC UA 地址空间中的某些信息，但它们的作用域、用途和唯一性保证是不同的。

---

## 🆚 `QualifiedName` (限定名称) 与 `NodeId` (节点 ID) 的主要区别

| 特性 | 🎯 `NodeId` (节点 ID) | ⚙️ `QualifiedName` (限定名称) |
| :--- | :--- | :--- |
| **OPC UA 类型** | `NodeId` (节点 ID) | `QualifiedName` |
| **作用域** | **全局唯一**（在整个 OPC UA 地址空间内，跨服务器、跨命名空间） | **命名空间局部唯一**（仅在其所属的 `NamespaceIndex` 内唯一） |
| **主要用途** | **寻址和引用**：用于读写数据、调用方法、订阅数据、建立引用等。 | **浏览和显示**：用于在地址空间中导航（`BrowseName`）和人类可读的名称。 |
| **组成结构** | `NamespaceIndex` (命名空间索引) + `Identifier` (标识符：可以是数字、字符串、GUID 或字节字符串) | `NamespaceIndex` (命名空间索引) + `Name` (文本名称) |
| **传输格式** | 是 OPC UA 服务请求和响应中 **定位节点** 的核心参数。 | 是节点的 **`BrowseName`** 属性的值，或用于事件过滤器中的 **字段选择**。 |

### 1. `NodeId`：寻址的身份证 (Identity Card)

`NodeId` 是 OPC UA 地址空间中节点的 **全局唯一标识符**。

* **唯一性：** 它保证在任何一个 OPC UA 服务器上，任何一个节点的 `NodeId` 都是唯一的。如果两个不同的服务器引用同一个标准 OPC UA 对象（如 `ServerStatus`），它们会拥有相同的 `NodeId` (`ns=0;i=2138`)。
* **用途：** 它是客户端与服务器交互（Read, Write, Call, CreateMonitoredItem 等）时，**精确指向** 目标节点的“地址”。

### 2. `QualifiedName`：浏览的标签 (Label/Tag)

`QualifiedName` 是一种 **可读的名称标签**，主要用于地址空间的导航。

* **唯一性：** 它只保证在同一个 **命名空间** (`NamespaceIndex`) 下是唯一的。
    * **示例：** 命名空间 1 下可以有一个 `Pump` 节点（`ns=1;s=Pump`），其 `BrowseName` 是 `ns=1;Pump`。
    * 命名空间 2 下也可以有一个 `Pump` 节点（`ns=2;s=Pump_B`），其 `BrowseName` 也是 `ns=2;Pump`。
    * 这两个 `Pump` 节点可以有不同的 `NodeId`，但它们的 `QualifiedName` 文本部分是相同的，通过 `NamespaceIndex` 区分。
* **用途：** 它是节点的 **`BrowseName`** 属性的值。当客户端通过 `Browse` 服务导航地址空间时，使用的就是这个 `QualifiedName`。此外，它也用于事件过滤器的 **`Select` 子句**，指定要返回的字段名称。

### 总结

可以将它们理解为：

* **`NodeId`** 就像一个人的 **身份证号码**：是全局唯一的，用于精准定位。
* **`QualifiedName`** 就像一个人的 **名字**：可能在不同省份（命名空间）有重名，需要结合省份（NamespaceIndex）才能确定局部唯一性，主要用于日常交流和查找。
