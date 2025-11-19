
# UaSettings


```c++
void ClientSampleConfig::loadConfiguration(
    const UaString& sIniFileName,
    const UaString& sAppPath)
{
    m_sConfigFile = sIniFileName;

    UaVariant     value;
    UaString      sTempKey;
    UaString      sTempValue;
    int           i         = 0;
    UaSettings*   pSettings = 0;
    // 创建对象
    pSettings = new UaSettings(sIniFileName);


    // 读节点 [UaClientConfig]
    pSettings->beginGroup("UaClientConfig"); // [UaClientConfig]
        // Server URLs
        //  读键值对
    value = pSettings->value("DiscoveryURL", UaString("opc.tcp://localhost:4840")); // DiscoveryURL=opc.tcp://localhost:4840
    m_sDiscoveryUrl = value.toString();
    value = pSettings->value("ServerUrl", UaString("opc.tcp://localhost:48010"));
    m_sDefaultServerUrl = value.toString();
    /// ....
    /// 读 带有符号‘/’的键值对
    pSettings->beginGroup("NS1"); // NS1/..
    /// 读 带有符号‘/’的键值对 结束
    pSettings->endGroup(); // NS1
    // 读节点 [UaClientConfig] 结束
    pSettings->endGroup(); 
}
```


# 
### UaNodeId UaNodeId::fromXmlString	(	UaString 	xmlString	)	
```C++
UaNodeId myNodeId;
UaString sNodeId("ns=2;s=MyNode");
myNodeId = UaNodeId::fromXmlString(sNodeId);

```

### changeType()
```c++
OpcUa_StatusCode UaVariant::changeType	(	OpcUa_BuiltInType 	type,
OpcUa_Boolean 	toArray 
)	
更改参数类型。

参数
类型	那种需要改变的类型。
toArray	如果设置为数组，则为 true;如果不是，则为 false。
返回
OPC UA状态代码。
Change the type of a parameter.

Parameters
type	the type to change.
toArray	true if set to array false if not.
Returns
the OPC UA Status code.
```

### OpcUa_IsGood 宏函数，解决状态码是否为good
```
OpcUa_IsGood
```

加载写值功能的配置代码,包含节点ID,数据类型，写入值。
配置文件 sampleconfig.ini
```c++
    UaVariant value;
    // *** New code begins ***
    OpcUa_Byte byteVal;
    // *** New code ends ***

    // *** New code begins ***
    // Read NodeIds, DataTypes and Values to use for writing
    m_nodesToWrite.clear();
    pSettings->beginGroup("NodesToWrite"); // NodesToWrite/Variable01
    value = pSettings->value("size", (OpcUa_UInt32)0);
    value.toUInt32(size);
    // NodeIds //将配置中的节点转换为 m_nodesToWrite 
    m_nodesToWrite.resize(size);
    for (i=0; i<size;i++)
    {
        sTempKey = UaString("Variable0%1").arg((int)i); // Variable01 Variable02
        value = pSettings->value(sTempKey, UaString("")); // ns=0;s=...
        UaNodeId::fromXmlString(value.toString()).copyTo(&m_nodesToWrite[i]); // 将字符串转换为节点对象。拷贝到类成员变量 m_nodesToWrite
    }
    // DataTypes //将配置中的类型转换为 writeDataTypes 
    UaByteArray writeDataTypes; // 这是临时变量，不是成员变量
    writeDataTypes.resize(size);
    for (i=0; i<size;i++)
    {
        sTempKey = UaString("DataType0%1").arg((int)i);
        value = pSettings->value(sTempKey, UaString(""));
        value.toByte(byteVal); 
        writeDataTypes[(int)i] = byteVal;
    }
    // Values //将配置中的写入值转换为 m_writeValues 
    m_writeValues.resize(size);
    for (i=0; i<size;i++)
    {
        sTempKey = UaString("Value0%1").arg((int)i);
        value = pSettings->value(sTempKey);
        // convert the value to the correct type
        OpcUa_BuiltInType type = (OpcUa_BuiltInType)(char)writeDataTypes[(int)i];
        if (OpcUa_IsGood(value.changeType(type, OpcUa_False))) //读到的字符串根据类型转换。
        {
            value.copyTo(&m_writeValues[i]);
        }
        else
        {
            printf("Cannot convert variant value: %s\n", value.toString().toUtf8());
        }
    }
    pSettings->endGroup();  // NodesToWrite
    // *** New code ends ***
```


```
◆getNamespaceTable（）

UaStringArray UaSession::getNamespaceTable	(		)	const

UaStringArray UaSession：：getNamespaceTable（获取NamespaceTable）	(		)	常量

获取服务器 NamespaceArray 属性值。
在连接和重新连接时，服务器读取命名空间阵列。它可以通过 updateNamespaceTable（） 的方法请求进行更新。
```



```
链接服务器后，更新命名空间所有
m_pConfiguration->updateNamespaceIndexes(m_pSession->getNamespaceTable());


```

```c++
void SampleClient::connectionStatusChanged(
    OpcUa_UInt32             clientConnectionId,
    UaClient::ServerStatus   serverStatus)
{
    OpcUa_ReferenceParameter(clientConnectionId);
 
    printf("-------------------------------------------------------------\n");
    switch (serverStatus)
    {
    case UaClient::Disconnected:
        printf("Connection status changed to Disconnected\n");
        break;
    case UaClient::Connected:
        printf("Connection status changed to Connected\n");
        // *** New code begins ***
        if (m_serverStatus != UaClient::NewSessionCreated)
        {
            m_pConfiguration->updateNamespaceIndexes(m_pSession->getNamespaceTable());
        }
        // *** New code ends ***
        break;
    case UaClient::ConnectionWarningWatchdogTimeout:
        printf("Connection status changed to ConnectionWarningWatchdogTimeout\n");
        break;
    case UaClient::ConnectionErrorApiReconnect:
        printf("Connection status changed to ConnectionErrorApiReconnect\n");
        break;
    case UaClient::ServerShutdown:
        printf("Connection status changed to ServerShutdown\n");
        break;
    case UaClient::NewSessionCreated:
        printf("Connection status changed to NewSessionCreated\n");
        // *** New code begins ***
        m_pConfiguration->updateNamespaceIndexes(m_pSession->getNamespaceTable());
        // *** New code ends ***
        break;
    }
    printf("-------------------------------------------------------------\n");
    // *** New code begins ***
    m_serverStatus = serverStatus;
    // *** New code ends ***
}
```


### updateNamespaceIndexes
```c++
UaStatus Configuration::updateNamespaceIndexes(const UaStringArray& namespaceArray)
{
    UaStatus result;
    OpcUa_UInt32 i, j;
    OpcUa_UInt32 size;
 
    // create mapping table
    size = m_namespaceArray.length();
    UaInt16Array mappingTable;
    mappingTable.resize(size);
 
    // fill mapping table
    for (i = 0; i < m_namespaceArray.length(); i++)
    {
        mappingTable[i] = (OpcUa_UInt16)i;
        // find namespace uri
        for (j = 0; j < namespaceArray.length(); j++)
        {
            UaString string1(m_namespaceArray[i]); // urn:UnifiedAutomation:CppDemoServer:BuildingAutomation
            UaString string2(namespaceArray[j]);
            if (string1 == string2)
            {
                mappingTable[i] = (OpcUa_UInt16)j;
                break;
            }
        }
    }
 
    // update namespace array
    m_namespaceArray = namespaceArray;
    return result;
}
```

### 命名空间配置
```ini
NSArray/size            =3
NSArray/NameSpaceUri00  =http://www.unifiedautomation.com/DemoServer/
NSArray/NameSpaceUri01  =urn:UnifiedAutomation:CppDemoServer:BuildingAutomation
NSArray/NameSpaceUri02  =urn:UnifiedAutomation:CppDemoServer:UANodeSetXmlImport

```


# G:

这段代码片段展示了 OPC UA 客户端在建立新会话后，如何同步和更新本地缓存的 **命名空间索引（Namespace Indexes）**，以确保正确地解析和使用服务器上的 NodeId。

`updateNamespaceIndexes` 函数是整个同步过程的核心，确保客户端的索引与服务器的索引保持一致。

-----

## 💻 命名空间索引更新机制分析

### 1\. 触发时机分析：`NewSessionCreated`

  * **触发点:** `case UaClient::NewSessionCreated:`
  * **含义:** 当客户端与 OPC UA 服务器成功建立了一个新的会话 (`Session`) 时，会触发此状态。
  * **重要性:** 每次建立新会话（或重新连接）时，服务器的命名空间表 (`NamespaceArray`) **都可能发生变化**（例如，服务器加载了新的模块，导致命名空间 URI 的索引发生偏移）。因此，客户端必须在此时立即获取最新的命名空间表并更新本地配置。
  * **调用:** `m_pConfiguration->updateNamespaceIndexes(m_pSession->getNamespaceTable());`
      * `m_pSession->getNamespaceTable()`：从服务器获取最新的 **命名空间 URI 数组**（即新的 `NamespaceArray`）。
      * `m_pConfiguration->updateNamespaceIndexes(...)`：将这个新的 URI 数组传入配置对象进行处理。

-----

### 2\. `updateNamespaceIndexes` 函数分析

该函数的核心目标是 **创建旧索引到新索引的映射表**，并用新表替换旧表。

#### A. 输入与输出

  * **输入 (`namespaceArray`):** 从服务器获得的 **最新的** 命名空间 URI 数组。
  * **内部旧数据 (`m_namespaceArray`):** 客户端本地当前缓存的 **旧的** 命名空间 URI 数组。
  * **核心任务:** 确定旧的 URI 在新的数组中的位置。

#### B. 核心算法流程

| 变量 | 含义 |
| :--- | :--- |
| `m_namespaceArray` | 客户端旧的 URI 列表 (旧索引 $i$) |
| `namespaceArray` | 服务器新的 URI 列表 (新索引 $j$) |
| `mappingTable` | **输出映射表**：存储旧索引 $i$ 对应的新索引 $j$ 的值。 |

1.  **初始化映射表 (`mappingTable`):**

    ```cpp
    mappingTable.resize(size);
    // for (i = 0; i < m_namespaceArray.length(); i++) { mappingTable[i] = (OpcUa_UInt16)i; }
    ```

      * 映射表的大小等于旧的 URI 数组大小。
      * 初始时，`mappingTable[i] = i`，假设旧索引 $i$ 保持不变。

2.  **创建映射关系 (嵌套循环):**

    ```cpp
    // for (i = 0; ... ) // 遍历旧 URI (m_namespaceArray)
    // {
    //     ...
    //     for (j = 0; ... ) // 遍历新 URI (namespaceArray)
    //     {
    //         if (string1 == string2) // 找到匹配的 URI
    //         {
    //             mappingTable[i] = (OpcUa_UInt16)j; // 更新：旧索引 i 对应新索引 j
    //             break;
    //         }
    //     }
    // }
    ```

      * 外层循环 (索引 $i$): 遍历本地旧的 URI。
      * 内层循环 (索引 $j$): 遍历服务器返回的新的 URI。
      * **匹配:** 如果 `m_namespaceArray[i]` 的 URI 字符串与 `namespaceArray[j]` 的 URI 字符串相同，则说明这个命名空间已经移动到了新的索引 $j$。
      * **更新映射:** 将 `mappingTable[i]` 设置为 $j$。

3.  **更新本地数组:**

    ```cpp
    m_namespaceArray = namespaceArray;
    ```

      * 用服务器返回的 **新 URI 数组** 替换本地缓存的 **旧 URI 数组**。

#### C. 遗留问题：映射表的用途

**这段代码只生成了 `mappingTable`，但没有使用它。**

在完整的客户端实现中，`mappingTable` 至关重要：

  * **NodeId 更新:** 客户端必须遍历其所有内部缓存的 **NodeId**（例如，订阅的 NodeId、引用的 NodeId）并使用 `mappingTable` 来更新它们的 **`NamespaceIndex`**。
  * **如何使用:** 如果客户端缓存了一个 NodeId 的旧索引为 $i$，则其新的索引应该设置为 `mappingTable[i]`。

例如，如果旧的索引 $i=5$ 现在映射到新的索引 $j=7$ (`mappingTable[5] == 7`)，那么所有旧的 `ns=5;...` 节点 ID 都必须更新为 `ns=7;...`。

这段代码中的 `mappingTable` 只是被创建和填充了，但没有被用于后续的 NodeId 转换，这可能发生在配置类内的其他私有方法中。


问题：
``` 
示例中在updateNamespaceIndexes函数的添加了   mappingTable使用，请分析
 for (i = 0; i < m_nodesToRead.length(); i++)
    {
        m_nodesToRead[i].NamespaceIndex = mappingTable[m_nodesToRead[i].NamespaceIndex];
    }
```

# G:
您添加的这段代码是 **`updateNamespaceIndexes` 函数中创建的 `mappingTable` 的核心用途**，它解决了在服务器命名空间表更新后，保持客户端内部缓存的 `NodeId` 正确性的问题。

-----

## 💻 命名空间索引更新机制（完整分析）

### 1\. 关键上下文：`m_nodesToRead`

  * **`m_nodesToRead`**：这个变量通常是客户端内部维护的一个列表或数组，存储了客户端在下次调用 `Read` 服务时需要读取的 **节点 (`NodeId`)** 列表。这些 NodeId 可能是客户端在程序启动时配置好的，或者是通过浏览服务发现并缓存的。

### 2\. 添加的映射代码分析

```cpp
for (i = 0; i < m_nodesToRead.length(); i++)
{
    // 使用旧索引作为 mappingTable 的下标
    OpcUa_UInt16 oldIndex = m_nodesToRead[i].NamespaceIndex;

    // 获取新索引，并将其赋值回 NodeId 结构体
    m_nodesToRead[i].NamespaceIndex = mappingTable[oldIndex];
}
```

#### A. 逻辑流程

1.  **遍历缓存的 NodeId：** 循环遍历客户端配置中所有待读取的节点（`m_nodesToRead`）。
2.  **提取旧索引：** 对于当前 NodeId，取出它当前存储的 `NamespaceIndex` (即旧索引 $i$)。
3.  **查找新索引：** 使用旧索引 $i$ 作为 `mappingTable` 的下标。由于 `mappingTable[i]` 存储了该命名空间 URI 在服务器新表中的位置（新索引 $j$），因此 `mappingTable[i]` 即为 NodeId 应有的新索引。
4.  **更新 NodeId：** 将查找到的新索引 $j$ 赋值回 `m_nodesToRead[i].NamespaceIndex`。

#### B. 示例说明

假设：

  * 客户端旧命名空间表中的索引 `5` 对应 URI `"http://example.com/ModuleA"`。
  * 服务器新的命名空间表中，URI `"http://example.com/ModuleA"` 现在位于索引 `7`。
  * 因此，`mappingTable[5]` 的值被设置为 `7`。

当代码执行到 `m_nodesToRead[k]` 时：

  * 如果 `m_nodesToRead[k].NamespaceIndex` 是 `5` (旧索引)。
  * 执行 `m_nodesToRead[k].NamespaceIndex = mappingTable[5]`。
  * 结果：`m_nodesToRead[k].NamespaceIndex` 被更新为 `7` (新索引)。

### 3\. 架构重要性

这段代码体现了 OPC UA 客户端封装库的 **健壮性**：

  * **数据一致性：** 它确保了客户端内部缓存的所有 NodeId 都能在当前服务器会话中被正确识别和使用。
  * **透明化处理：** 对上层应用程序而言，这个索引变化是完全透明的。应用程序可以继续使用配置好的 NodeId 字符串或结构体，而无需担心服务器重启或配置变化导致的命名空间索引偏移问题。

**结论：** 这段代码成功地将本地旧的命名空间索引转换为服务器最新的命名空间索引，并应用到了需要读取的 NodeId 列表上，是 `updateNamespaceIndexes` 函数中 **最核心的 NodeId 修正逻辑**。