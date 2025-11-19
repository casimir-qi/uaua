
https://github.com/qt/qtopcua/commit/a95eec14091e21a3b7c5bde17cefe00e584d8322
# qAsConst -> std::as_const
从 qAsConst（） 移植到 std：：as_const（）
自Qt 6.0以来，我们一直要求C++17，而我们的qAsConst终于被使用
开始困扰我们（QTBUG-99313），是时候转移离开它了
现在。

因为 qAsConst 的语义和 std 完全一样 ：：as_const （down ）
要考虑处理、一致性和非例外性），其实有区别
不过是一次全球搜索与替换。

这是我们在开发中运行脚本的6.4版本重运行，以避免出现问题
在择优选择时，分支之间的冲突。

Port from qAsConst() to std::as_const()
We've been requiring C++17 since Qt 6.0, and our qAsConst use finally
starts to bother us (QTBUG-99313), so time to port away from it
now.

Since qAsConst has exactly the same semantics as std::as_const (down
to rvalue treatment, constexpr'ness and noexcept'ness), there's really
nothing more to it than a global search-and-replace.

This is a 6.4 re-run of the script we ran in dev, in order to avoid
conflicts between the branches when cherry-picking.

Change-Id: I18478918d6306d83985151d38babc8e077956fe9
Reviewed-by: Volker Hilsheimer <volker.hilsheimer@qt.io>



https://github.com/qt/qtopcua/commit/4a2b804e18bde7f289f0c4ebf565e18ce29e92bc

basyskom-jvoe
committed
on Nov 20, 2023

移除 UACPP 插件
统一自动化插件仅部分移植到Qt 6，之后大多未维护。
自5.15及以后引入的新功能仅适用于open62541后端。

[更新日志][Qt OPC UA]统一自动化后端已被移除。
 open62541 后端可以作为
 除了connectError（）信号外，其他都没有。

更改编号：I7e859133bb973256cdfe8eb9b6949379acb689b9
审核者：Qt CI 机器人<qt_ci_bot@qt-project.org>
审阅人：弗兰克·米尔科特<frank.meerkoetter@basyskom.com>
审阅人：弗拉基米尔·米年科·<vladimir.minenko@qt.io>

Remove the UACPP plugin
The Unified Automation plugin was only partially ported to Qt 6 and was since then mostly unmaintained.
New feature introduced since 5.15 and later were only available for the open62541 backend.

[ChangeLog][Qt OPC UA] The Unified Automation backend has been removed.
    The open62541 backend will be able to act as a drop-in replacement for
    everything except the connectError() signal.

Change-Id: I7e859133bb973256cdfe8eb9b6949379acb689b9
Reviewed-by: Qt CI Bot <qt_ci_bot@qt-project.org>
Reviewed-by: Frank Meerkoetter <frank.meerkoetter@basyskom.com>
Reviewed-by: Vladimir Minenko <vladimir.minenko@qt.io>