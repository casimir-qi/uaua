# 正则替换cite
#vscode 查找
\[cite: [^\]]*\]

## 正则表达式解释：
\[cite: ：匹配 [cite: ，其中 \[ 是因为方括号是正则表达式中的特殊字符，所以需要转义。
[^\]]*：匹配 [ 和 ] 之间的任意字符（不包括 ]），即捕获所有的数字、逗号或空格。
\]：匹配右方括号 ]。


### Linux指令
```sh
sed -i 's/\[cite: [^]]*\]//g' yourfile.md
```
### 解释：
-i：表示直接修改文件（原地编辑）。
's/\[cite: [^]]*\]//g'：正则表达式的意思是匹配形如 [cite: 11, 12] 的字符串并将其删除。
\[cite: ：匹配文本 [cite: 。
[^\]]*：匹配非 ] 的字符（即数字和逗号），直到遇到 ]。
\]：匹配右方括号 ]。

```python
import re

# 读取文件
with open('yourfile.md', 'r', encoding='utf-8') as file:
    content = file.read()

# 使用正则表达式删除匹配的文本
content = re.sub(r'\[cite: [^\]]*\]', '', content)

# 将修改后的内容写回文件
with open('yourfile.md', 'w', encoding='utf-8') as file:
    file.write(content)

```

# sort排序

在 Linux 或 macOS 中，你可以使用 `sort` 命令按文件的第 2 列进行排序。可以通过 `-k` 参数指定排序的列。

### 示例命令：
```bash
sort -k2 file.txt
```

### 解释：
- `-k2`：表示按第二列进行排序（默认情况下，列是以空格或制表符为分隔符的）。
- `file.txt`：是你要排序的文件名。

### 其他常见选项：
1. **按数字排序**（如果第二列是数字）：
   ```bash
   sort -k2,2n file.txt
   ```
   - `-n`：表示按数字大小排序。

2. **按降序排序**：
   ```bash
   sort -k2,2r file.txt
   ```
   - `-r`：表示降序排序。

3. **排序并覆盖原文件**（使用 `-o`）：
   ```bash
   sort -k2 file.txt -o file.txt
   ```

### 示例文件（`file.txt`）：
```
John 25
Alice 30
Bob 22
```

### 排序后的输出（按第二列升序排序）：
```
Bob 22
John 25
Alice 30
```

### 按多列排序：
如果你想按第二列和第三列排序，可以使用：
```bash
sort -k2,2 -k3,3 file.txt
```

这会先按第二列排序，如果第二列有相同值，则会根据第三列进一步排序。

这样，你可以灵活地使用 `sort` 命令进行列排序！