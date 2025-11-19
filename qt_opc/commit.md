
https://github.com/qt/qtopcua/commit/a95eec14091e21a3b7c5bde17cefe00e584d8322
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