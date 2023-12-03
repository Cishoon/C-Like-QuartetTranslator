
在提交的源代码文件的 ` output` 文件夹中，已经包含windows环境可执行文件   `LR1Parser.exe`.

如需编译可使用如下命令：

- Windows：

  ```
  g++ -std=c++17 -O2 .\src\main.cpp .\src\Lexer.cpp .\src\LR1Parser.cpp -o .\output\LR1Parser.exe
  ```
- Linux/MacOS：

  ```
  g++ -std=c++17 -O2 ./src/main.cpp ./src/Lexer.cpp ./scr/LR1Parser.cpp -o ./output/LR1Parser
  ```

(注：g++需要支持c++17)

运行方法：

```
LR1Parser.exe <输入文件路径> <文法文件路径>
```

例如：

```
LR1Parser.exe ./test/input/input.txt ./test/grammer/grammer.txt
```

或直接运行 `build.bat` (Windows) `bash build.sh` (Linux/MacOS)，包含了编译和运行的过程。


详细内容见报告
