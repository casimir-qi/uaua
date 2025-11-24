1 QVector 转 QList;

qAsConst 转 std::as_const

2 要求编译版本为Qt6
#if QT_VERSION >= 0x060000

#else

#endif

3 
qt_make_unique -> std::make_unique


static LoadedOpenSsl loadOpenSsl()
{
    LoadedOpenSsl result = {qt_make_unique<QLibrary>(), qt_make_unique<QLibrary>()};
}

QJsonObject -> QCborarray


### QJsonObject
JSON 对象是键值对的列表，其中键是唯一的字符串，值由 QJsonValue 表示。

QJsonObject 可以与 QVariantMap 相互转换。您可以从中查询包含 size ()、 insert () 和 remove () 条目的 (键，值) 对的数量，并使用标准 C++ 迭代器模式迭代其内容。

QCborMap 
## QCborArray
Qt6的类
此类可用于保存 CBOR 中的一个顺序容器（数组）。CBOR 是简洁二进制对象表示，是一种非常紧凑的二进制数据编码形式，是 JSON 的超集。它由 IETF 受限 RESTful 环境 (CoRE) WG 创建，该工作组已在许多新的 RFC 中使用了它。它旨在与 CoAP protocol 一起使用。


## 新增类
```
QOpcUaConnectionSettings

QOpcUaDataValue

QOpcUaHistoryData

QOpcUaHistoryReadRawRequest

QOpcUaHistoryReadResponse

QOpcUaHistoryReadResponseImpl

QOpcUaLiteralOperand
```


文件 qopcuatype.h 增加枚举
```c++
enum class ReferenceTypeId
{
    HasArgumentDescription = 129,
    HasOptionalInputArgumentDescription = 131,
....
HasPubSubConnection = 14476,
    DataSetToWriter = 14936,
    HasGuard = 15112,
    HasDataSetWriter = 15296,
    HasDataSetReader = 15297,
    HasAlarmSuppressionGroup = 16361,
    AlarmGroupMember = 16362,
    HasEffectDisable = 17276,
    HasDictionaryEntry = 17597,
    HasInterface = 17603,
    HasAddIn = 17604,
    HasEffectEnable = 17983,
    HasEffectSuppressed = 17984,
    HasEffectUnsuppressed = 17985,
    HasWriterGroup = 18804,
    HasReaderGroup = 18805,
    AliasFor = 23469,
}
```


```
    friend Q_OPCUA_EXPORT bool operator==(const QOpcUaReadItem &lhs,
                                          const QOpcUaReadItem &rhs) noexcept;
    friend inline bool operator!=(const QOpcUaReadItem &lhs, const QOpcUaReadItem &rhs) noexcept
    {
        return !(lhs == rhs);
    }
```


文件 qopcuanodeids.h  枚举有较大改变，有删除、修改、新增。

文件 
qopcuanode.h 
增加函数 读取历史Raw
QOpcUaHistoryReadResponse *readHistoryRaw(const QDateTime &startTime, const QDateTime &endTime, quint32 numValues, bool returnBounds);

文件增加
qopcuaclientimpl_p.h
virtual QOpcUaHistoryReadResponse *readHistoryData(const QOpcUaHistoryReadRawRequest &request) = 0;