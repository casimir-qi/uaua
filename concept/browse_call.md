# 测试实例

```c++

/*============================================================================
 * exploreAddressSpace - explore the OPC UA Server addressspace
 *===========================================================================*/
void exploreAddressSpace(const UaNodeId& startingNode, unsigned int level)
{
    UaStatus                status;
    UaByteString            continuationPoint;
    UaReferenceDescriptions referenceDescriptions;
    ServiceSettings         serviceSettings;
    BrowseContext           browseContext;

    /*********************************************************************
     Browse Server
    **********************************************************************/
    status = g_pUaSession->browse(
        serviceSettings,
        startingNode,
        browseContext,
        continuationPoint,
        referenceDescriptions);
    /*********************************************************************/

    if ( status.isBad() )
    {
        printf("** Error: UaSession::browse of NodeId = %s failed [ret=%s]\n", startingNode.toFullString().toUtf8(), status.toString().toUtf8());
        return;
    }
    else
    {
        OpcUa_UInt32 i, j;
        for (i=0; i<referenceDescriptions.length(); i++)
        {
            printf("node: ");
            for (j=0; j<level; j++) printf("  ");
            UaNodeId referenceTypeId(referenceDescriptions[i].ReferenceTypeId);
            printf("[Ref=%s] ", referenceTypeId.toString().toUtf8() );
            UaQualifiedName browseName(referenceDescriptions[i].BrowseName);
            printf("%s ( ", browseName.toString().toUtf8() );
            if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_Object) printf("Object ");
            if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_Variable) printf("Variable ");
            if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_Method) printf("Method ");
            if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_ObjectType) printf("ObjectType ");
            if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_VariableType) printf("VariableType ");
            if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_ReferenceType) printf("ReferenceType ");
            if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_DataType) printf("DataType ");
            if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_View) printf("View ");
            UaNodeId nodeId(referenceDescriptions[i].NodeId.NodeId);
            printf("[NodeId=%s] ", nodeId.toFullString().toUtf8() );
            printf(")\n");

            exploreAddressSpace(referenceDescriptions[i].NodeId.NodeId, level+1 );
        }

        // Check if the continuation point was set -> call browseNext
        while ( continuationPoint.length() > 0 )
        {
            /*********************************************************************
             Browse remaining nodes in the Server
            **********************************************************************/
            status = g_pUaSession->browseNext(
                serviceSettings,
                OpcUa_False,
                continuationPoint,
                referenceDescriptions);
            /*********************************************************************/

            if ( status.isBad() )
            {
                printf("** Error: UaSession::browse of NodeId = %s failed [ret=%s] **\n", startingNode.toFullString().toUtf8(), status.toString().toUtf8());
                return;
            }
            else
            {
                for (i=0; i<referenceDescriptions.length(); i++)
                {
                    printf("node: ");
                    for (j=0; j<level; j++) printf("  ");
                    UaNodeId referenceTypeId(referenceDescriptions[i].ReferenceTypeId);
                    printf("[Ref=%s] ", referenceTypeId.toString().toUtf8() );
                    UaQualifiedName browseName(referenceDescriptions[i].BrowseName);
                    printf("%s ( ", browseName.toString().toUtf8() );
                    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_Object) printf("Object ");
                    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_Variable) printf("Variable ");
                    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_Method) printf("Method ");
                    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_ObjectType) printf("ObjectType ");
                    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_VariableType) printf("VariableType ");
                    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_ReferenceType) printf("ReferenceType ");
                    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_DataType) printf("DataType ");
                    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_View) printf("View ");
                    UaNodeId nodeId(referenceDescriptions[i].NodeId.NodeId);
                    printf("[NodeId=%s] ", nodeId.toFullString().toUtf8() );
                    printf(")\n");

                    exploreAddressSpace(referenceDescriptions[i].NodeId.NodeId, level+1 );
                }
            }
        }
    }
    return;
}

```

这是一个经典的 **OPC UA 客户端递归浏览地址空间 (Address Space)** 的实现函数，它基于 UaStack/Unified Automation C++ SDK。

该函数 **`exploreAddressSpace`** 的目的是从一个指定的起始节点开始，遍历其所有引用的节点，并将找到的节点信息以树形结构打印出来。

-----

## 💻 `exploreAddressSpace` 函数详细分析

### 1\. 函数定义与参数

```cpp
void exploreAddressSpace(const UaNodeId& startingNode, unsigned int level)
```

  * **`startingNode` (输入):** 这是一个 **`UaNodeId`** 类型，指定了本次浏览操作的起始节点。例如，第一次调用时通常传入 `ObjectsFolder` (ns=0;i=84)。
  * **`level` (输入):** 一个整数，表示当前节点在地址空间树中的深度。主要用于控制输出的缩进，以展示清晰的层次结构。

### 2\. 核心 OPC UA 浏览服务调用 (`browse`)

函数的核心是调用 OPC UA 的 `Browse` 服务。

```cpp
status = g_pUaSession->browse(
    serviceSettings,
    startingNode,
    browseContext,
    continuationPoint,
    referenceDescriptions);
```

  * **`g_pUaSession`:** 全局会话指针，表示客户端与服务器建立的连接。
  * **`startingNode`:** 指定要浏览的节点。
  * **`browseContext`:** 包含浏览操作的详细参数（如引用类型、方向、是否返回类型定义等），但在这个示例中，它以默认设置使用。
  * **`referenceDescriptions` (输出):** **最重要的输出**，是一个数组，包含服务器返回的关于 `startingNode` 的所有引用信息（即它的子节点或关联节点）。
  * **`continuationPoint` (输出):** 字节串，用于处理服务器返回结果过多的情况。如果服务器一次不能返回所有结果，它会设置这个点，客户端需要调用 `browseNext` 来获取剩余部分。

### 3\. 错误处理

```cpp
if ( status.isBad() ) { ... return; }
```

如果 `browse` 调用失败（`status` 是 Bad），函数将打印错误信息并立即返回，停止进一步的递归浏览。

### 4\. 结果处理和递归遍历（主要循环）

当 `browse` 成功后，代码遍历 `referenceDescriptions` 数组，打印信息并进行递归：

#### A. 打印节点信息

```cpp
for (i=0; i<referenceDescriptions.length(); i++)
{
    // ... 打印缩进 ...
    // 打印引用的类型 (ReferenceTypeId)
    UaNodeId referenceTypeId(referenceDescriptions[i].ReferenceTypeId);
    printf("[Ref=%s] ", referenceTypeId.toString().toUtf8() );
    
    // 打印节点的浏览名称 (BrowseName)
    UaQualifiedName browseName(referenceDescriptions[i].BrowseName);
    printf("%s ( ", browseName.toString().toUtf8() );
    
    // 打印节点的节点类型 (NodeClass)
    // 使用位掩码 (bitmask) 判断节点类型（Object, Variable, Method, etc.）
    if (referenceDescriptions[i].NodeClass & OpcUa_NodeClass_Object) printf("Object ");
    // ... 其他 NodeClass 检查 ...
    
    // 打印节点的 NodeId
    UaNodeId nodeId(referenceDescriptions[i].NodeId.NodeId);
    printf("[NodeId=%s] ", nodeId.toFullString().toUtf8() );
    printf(")\n");
    // ...
```

这段代码将服务器返回的每个引用（子节点）的关键信息（引用类型、浏览名、节点类、NodeId）格式化并打印到控制台。

#### B. 递归调用

```cpp
exploreAddressSpace(referenceDescriptions[i].NodeId.NodeId, level+1 );
```

这是实现深度遍历的关键。对于当前层级（`level`）找到的每一个子节点，函数都会以该子节点的 **`NodeId`** 为新的起始节点，并增加层级（`level+1`）来 **递归调用自身**，从而继续探索地址空间的下一层。

### 5\. 处理 ContinuationPoint (`browseNext` 循环)

```cpp
while ( continuationPoint.length() > 0 ) { ... }
```

这个 `while` 循环是处理 OPC UA 分页结果的标准机制，保证地址空间中所有子节点都能被发现：

1.  **检查条件：** 如果第一次 `browse` 调用后 `continuationPoint` 字节串不为空，说明还有更多结果未返回。
2.  **调用 `browseNext`：**
    ```cpp
    status = g_pUaSession->browseNext(
        serviceSettings,
        OpcUa_False, // 释放 ContinuationPoint 的标志
        continuationPoint, // 传入上一次返回的点
        referenceDescriptions); // 输出剩余的引用
    ```
      * `browseNext` 接收上一次的 `continuationPoint` 作为输入，告诉服务器“请给我剩余的结果”。
3.  **循环处理：** 循环内部的代码与第 4 节的第一个 `for` 循环功能相同：打印新获取的 `referenceDescriptions` 中的节点信息，并对每个节点进行递归调用。
4.  **终止：** 循环将继续执行，直到服务器在 `browseNext` 调用后返回一个空的 `continuationPoint`，表示所有结果已获取。

### 总结机制

`exploreAddressSpace` 函数通过 **深度优先搜索 (DFS)** 的方式遍历服务器地址空间：

1.  **浏览** 当前节点。
2.  **处理** 所有结果（包括使用 `browseNext` 处理分页结果）。
3.  对每一个子节点 **递归调用** 自身。
4.  **返回**，直到整个树形结构被完全遍历。
