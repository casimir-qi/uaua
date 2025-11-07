```
/** \struct OpcUa_NodeId
 *  An identifier for a node in the address space of an OPC UA Server.
 * 
 * A Server shall persist the NodeId of a Node, that is, it is not
 * allowed to generate new NodeIds when rebooting. The server is not
 * allowed to change the namespace index for a namespace URI as long as
 * the server is not restarted, but new namespaces can be added.  The
 * namespace index for a namespace URI may change after a server
 * restart.
 *
 * Clients must be prepared for changes of the namespace index in a
 * NodeId and must persist their own namespace table together with the
 * stored NodeIds for a server.
 *
 * The following three elements identify a Node within a Server:
 *
 * Name           | Type   | Description
 * ---------------|--------|------------------------------------------------------------------
 * namespaceIndex | UInt16 | The index for a namespace URI used in an OPC UA server
 * identifierType | enum   | The format and data type of the identifier
 * identifier     | *      | The identifier for a node in the address space of an OPC UA server
 *
 * <b>Namespace Index</b>
 *
 * The namespace is a URI that identifies the naming authority
 * responsible for assigning the identifier element of the
 * NodeId. Naming authorities include the local server, the underlying
 * system, standards bodies and consortia. It is expected that most
 * nodes will use the URI of the server or of the underlying system.
 *
 * Using a namespace URI allows multiple OPC UA servers attached to
 * the same underlying system to use the same identifier to identify
 * the same object. This enables clients that connect to those Servers
 * to recognise Objects that they have in common.
 *
 * Namespace URIs are identified by numeric values in OPC UA services
 * to permit more efficient transfer and processing (e.g. table
 * lookups). The numeric values used to identify namespaces correspond
 * to the index into the NamespaceArray. The NamespaceArray is a
 * variable that is part of the Server object in the address space.
 *
 * The URI for the OPC UA namespace is
 * "http://opcfoundation.org/UA/". Its corresponding index in the
 * namespace table is 0.
 *
 * The Namespace URI is case sensitive.
 *
 * <b>%Identifier Type</b>
 *
 * The IdentifierType element identifies the type of the NodeId, its
 * format and its scope. The following IdentifierTypes are available:
 *
 * Value                        | Description
 * -----------------------------|--------------------------------------------
 * OpcUa_IdentifierType_Numeric | Numeric value
 * OpcUa_IdentifierType_String  | String value
 * OpcUa_IdentifierType_Guid    | Globally Unique %Identifier
 * OpcUa_IdentifierType_Opaque  | Namespace specific format in a ByteString
 *
 * <b>%Identifier value</b>
 *
 * The identifier value element is used within the context of the
 * first two elements to identify a node. Its data type and format is
 * defined by the identifierType.
 *
 * Numeric identifiers are typically used for static namespaces or for
 * NodeIds in generated code.  Such identifiers in generated code have
 * typically defines generated which makes them easy to use and read
 * in code.
 *
 * String identifiers are typically used for nodes that are
 * dynamically generated during system configuration. The string is
 * either a path of nodes with a server specific delimiters or a
 * server specific addressing schema contained in the string.
 *
 * String identifiers are case sensitive. That is, clients shall
 * consider them case sensitive.  Servers are allowed to provide
 * alternative NodeIds and using this mechanism severs can handle
 * NodeIds as case insensitive.
 *
 * Normally the scope of NodeIds is the server in which they are
 * defined. For certain types of NodeIds, NodeIds can uniquely
 * identify a Node within a system, or across systems (e.g. GUIDs).
 * System-wide and globally-unique identifiers allow clients to track
 * Nodes, such as work orders, as they move between OPC UA servers as
 * they progress through the system.
 *
 * Opaque identifiers are identifiers that are free-format byte
 * strings that might or might not be human interpretable.
 *
 * Identifier values of IdentifierType STRING are restricted to 4096
 * characters. Identifier values of IdentifierType OPAQUE are
 * restricted to 4096 bytes.
 *
 * A null NodeId has special meaning. For example, many services
 * define special behaviour if a null NodeId is passed as a
 * parameter. A null NodeId always has a NamespaceIndex equal to 0. A
 * node in the address space cannot have a null NodeId.  Each
 * identifier type has a set of identifier values that represent a
 * null NodeId:
 *
 * %Identifier Type | %Identifier
 * -----------------|-------------------------------------------------
 * NUMERIC          | 0
 * STRING           | A null or Empty String ("")
 * GUID             | A Guid initialised with zeros (e.g. 00000000-0000-0000-0000-000000)
 * OPAQUE           | A ByteString with Length=0
 *
 * \var OpcUa_NodeId::NamespaceIndex
 * The index for a namespace URI.
 *
 * \var OpcUa_NodeId::IdentifierType
 * The format and data type of the identifier.
 *
 * \union OpcUa_NodeId::Identifier
 * The identifier for a Node in the AddressSpace of an OPC UA Server.
 *
 */
typedef struct _OpcUa_NodeId
{
    OpcUa_UInt16 IdentifierType;
    OpcUa_UInt16 NamespaceIndex;

    union _Identifier
    {
        OpcUa_UInt32     Numeric;    /**< \memberof OpcUa_NodeId::Identifier Numeric value */
        OpcUa_String     String;     /**< \memberof OpcUa_NodeId::Identifier String value */
        OpcUa_Guid*      Guid;       /**< \memberof OpcUa_NodeId::Identifier Globally Unique %Identifier */
        OpcUa_ByteString ByteString; /**< \memberof OpcUa_NodeId::Identifier Namespace specific format in a ByteString */
    }
    Identifier;
}
OpcUa_NodeId;
这是节点ID的源文档，撰写一份节点ID的文档
```


好的，根据您提供的 UaStack C 结构体定义和详细的 OPC UA 规范描述，我将为您撰写一份关于 **`OpcUa_NodeId`（节点 ID）** 的技术文档。

-----

# 📝 `OpcUa_NodeId`（节点 ID）技术文档

## 概述

**`OpcUa_NodeId`** 是 OPC UA 协议中用于 **唯一标识** 服务器地址空间中一个节点的关键数据结构。它是一个节点的“身份证号码”，保证在服务器的生命周期内不变，并用于客户端与服务器之间所有与寻址相关的服务交互（如读写、订阅、浏览等）。

## 结构组成

`OpcUa_NodeId` 由三个核心元素组成，共同确保了其全局唯一性和灵活性：

| 名称 | 类型 | 描述 |
| :--- | :--- | :--- |
| **`namespaceIndex`** | `UInt16` | 命名空间索引，标识名称的权威性。 |
| **`identifierType`** | `enum` | 标识符的格式和数据类型（Numeric, String, Guid, Opaque）。 |
| **`identifier`** | `*` (Union) | 节点的实际标识值，类型取决于 `identifierType`。 |

```c
typedef struct _OpcUa_NodeId
{
    OpcUa_UInt16 IdentifierType; // 标识符类型
    OpcUa_UInt16 NamespaceIndex; // 命名空间索引
    union _Identifier
    {
        OpcUa_UInt32     Numeric;     
        OpcUa_String     String;
        OpcUa_Guid*      Guid;
        OpcUa_ByteString ByteString;
    }
    Identifier;
}
OpcUa_NodeId;
```

-----

## 1\. Namespace Index（命名空间索引）

命名空间机制是 OPC UA 实现 **可扩展性** 和 **名称权威性** 的基础。

  * **作用：** `NamespaceIndex` 引用一个 **URI (Uniform Resource Identifier)**，这个 URI 标识了负责定义 `NodeId` 中 `identifier` 部分的命名权威机构（如 OPC 基金会、服务器供应商或底层系统）。
  * **客户端职责：** 客户端必须在连接服务器时读取 **`NamespaceArray`** 变量（位于 Server 对象下），并维护自己的命名空间表，因为服务器重启后，索引与 URI 的对应关系可能会发生变化。
  * **OPC UA 标准命名空间：** 索引 `0` 始终保留给 OPC UA 规范，对应的 URI 是 `"http://opcfoundation.org/UA/"`。

-----

## 2\. Identifier Type（标识符类型）

`IdentifierType` 决定了 `Identifier` 联合体的哪一个成员是有效的，以及该标识符的格式和数据类型。

| 值 | 类型 | 描述 | 典型用途 |
| :--- | :--- | :--- | :--- |
| `OpcUa_IdentifierType_Numeric` | `OpcUa_UInt32` | **数字值。** | 用于静态命名空间或代码生成中的易读 ID。 |
| `OpcUa_IdentifierType_String` | `OpcUa_String` | **字符串值。** | 用于系统配置过程中动态生成的节点，可以是路径或服务器特定的寻址方案。**区分大小写**，限制 4096 字符。 |
| `OpcUa_IdentifierType_Guid` | `OpcUa_Guid*` | **全局唯一标识符。** | 保证跨系统、跨服务器的全局唯一性，适用于跟踪如工单等在系统中移动的对象。 |
| `OpcUa_IdentifierType_Opaque` | `OpcUa_ByteString` | **不透明字节串。** | 服务器特定的、可能是非人类可读的格式。限制 4096 字节。 |

-----

## 3\. Identifier Value（标识符值）

这是 NodeId 的核心内容，根据 `IdentifierType` 采用不同的数据类型和格式。

  * **唯一性范围：** NodeId 的唯一性范围通常是定义它的服务器。然而，`GUID` 等类型支持系统级甚至全球范围的唯一性。
  * **字符串标识符：** 客户端必须将其视为 **区分大小写** 的。服务器可以通过提供替代的 NodeId 来实现对大小写不敏感的处理。

## 4\. Null NodeId（空节点 ID）

一个 **空 NodeId** 具有特殊含义，并在许多 OPC UA 服务中用于定义特殊行为。

  * **特征：** 空 NodeId 始终具有 `NamespaceIndex` 等于 **0**。
  * **表示方式：** 每种标识符类型都有对应的空值表示：

| Identifier Type | Identifier Value for Null |
| :--- | :--- |
| `NUMERIC` | `0` |
| `STRING` | 空字符串 (`""`) 或 Null 字符串 |
| `GUID` | 所有位清零的 GUID |
| `OPAQUE` | 长度为 0 的 ByteString |

**注意：** 地址空间中的任何节点都 **不能** 拥有一个空 NodeId。

-----

**下一步我可以为您做些什么？**

  * 您希望将这份文档的重点放在 **NodeId 的客户端持久化策略** 上吗？
  * 或者，您想讨论在 UaStack 中如何构造具有不同 `IdentifierType` 的 `UaNodeId` 实例？