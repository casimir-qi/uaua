如何判断一个lib是MD的还是MT的


这是一个非常好的问题。在 Windows 平台（尤其是使用 MSVC 编译器时），`lib` 文件本身只是一个包含对象文件 (`.obj`) 的归档，这些 `.obj` 文件才是包含编译器选项信息的实体。

要判断一个静态库（`.lib` 文件）或对象文件（`.obj` 文件）是使用 `/MD` (Dynamic) 还是 `/MT` (Static) 运行时库编译的，最可靠、最常用的方法是使用 **Visual Studio 的命令行工具**。

### 🛠️ 使用 `DUMPBIN` 工具进行判断

`DUMPBIN` 是 Microsoft Visual Studio 附带的一个工具，用于显示 PE (Portable Executable) 文件（包括 `.exe`, `.dll`, `.lib`, `.obj`）的二进制信息。我们可以用它来检查对象文件中嵌入的 **`RuntimeLibrary`** 标志。

#### 步骤一：启动命令行环境

启动 **"x64 Native Tools Command Prompt for VS 2022"** 或 **"Developer Command Prompt for VS 2022"**。这是确保 `dumpbin.exe` 和其他必要工具在你的路径 (Path) 中的最简单方式。

#### 步骤二：使用 DUMPBIN 检查

使用 `DUMPBIN` 配合 `/LINKERMEMBER` 和 `/ALL` 选项来分析 `.lib` 文件。我们关注的是编译器的**注释记录 (Comment Records)**，其中包含了 `RuntimeLibrary` 的信息。

**命令格式：**

```bash
DUMPBIN /LINKERMEMBER:1 /ALL <path_to_your_library.lib>
```

**示例：**

```bash
DUMPBIN /LINKERMEMBER:1 /ALL my_static_library.lib
```

#### 步骤三：分析输出结果

在 `DUMPBIN` 的输出中，找到包含 **`$If`** 或 **`Linker Directives`** 的部分，并查找与 `RuntimeLibrary` 相关的记录。

| 检测到的值 | 含义 (RuntimeLibrary) | C/C++ 选项 |
| :--- | :--- | :--- |
| **`msvcrt.lib`** | 动态链接 (Release) | **`/MD`** |
| **`msvcrtd.lib`** | 动态链接 (Debug) | **`/MDd`** |
| **`libcmt.lib`** | 静态链接 (Release) | **`/MT`** |
| **`libcmtd.lib`** | 静态链接 (Debug) | **`/MTd`** |

**`/MT` 库的典型输出：**

您可能会看到类似这样的行，表明它依赖静态运行时库：

```
COMDAT; symbol name: ?_Stinit@?A0x2ff2e4d0@@YAXXZ (void __cdecl `anonymous namespace'::_Stinit(void))
  .drectve -_defaultlib:msvcrt.lib  /defaultlib:libcmt.lib 
                                    ^^^^^^^^^^^^^^^^^^^^^ 
                                     /MT 库的标志
```

**`/MD` 库的典型输出：**

您可能会看到类似这样的行，表明它依赖动态运行时库：

```
COMDAT; symbol name: ?_Stinit@?A0x2ff2e4d0@@YAXXZ (void __cdecl `anonymous namespace'::_Stinit(void))
  .drectve -_defaultlib:libcmt.lib  /defaultlib:msvcrt.lib 
                                    ^^^^^^^^^^^^^^^^^^^^^ 
                                     /MD 库的标志
```

-----

### 💡 **另一种快速（但非绝对可靠）的方法：文件内容搜索**

虽然不如 `DUMPBIN` 精确，但您可以尝试用文本编辑器或搜索工具在 `.lib` 文件的二进制内容中搜索上述提到的库名称：`msvcrt.lib`, `libcmt.lib`, `msvcrtd.lib`, 或 `libcmtd.lib`。

  * 如果搜索到 `msvcrt.lib` 或 `msvcrtd.lib`，则它很可能是 `/MD` (Dynamic)。
  * 如果搜索到 `libcmt.lib` 或 `libcmtd.lib`，则它很可能是 `/MT` (Static)。

**注意：** 这种方法并不总是有效，因为编译器可能会通过其他方式嵌入这些链接指令，但它提供了一个快速的初步检查。

-----

如果您的 `.lib` 文件包含大量 `.obj` 文件，您可能需要检查多个成员，但通常它们会有一致的设置。

您想让我演示一个具体的 `DUMPBIN` 输出示例，或者您想尝试分析您手头的一个 `.lib` 文件吗？

总结：
lib文件中包含一些字段：
RuntimeLibrary=XXXXX

|||
|-|-|
|MTd_StaticDebug |MDd_DynamicDebug|
||MD_DynamicRelease|
|MTd|MDD|

|||||||
|-|-|-|-|-|-|
|MT|MD|Static|Dynamic|Debug|Release|
|MT||||
|MD||||
|Static||||
|Dynamic||||
|MT|MD|Static|Dynamic|Debug|Release|
|MT|MD|Static|Dynamic|Debug|Release|

MT - MD
Static - Dynamic
Release - Debug

error LNK2038: 检测到“RuntimeLibrary”的不匹配项: 值“MD_DynamicRelease”不匹配值“MDd_DynamicDebug” （引用的是release模式，但设置成debug模式了） 

error LNK2038: 检测到“RuntimeLibrary”的不匹配项: 值“MT_StaticRelease”不匹配值“MTd_StaticDebug” （引用的是release模式，但设置成debug模式了） 

error LNK2038: 检测到“RuntimeLibrary”的不匹配项: 值“MT_StaticRelease”不匹配值“MD_DynamicRelease” （引用的是静态库，但设置成动态库了）