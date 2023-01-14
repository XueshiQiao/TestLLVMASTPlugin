# TestLLVMASTPlugin

This project is my attempt to explore LLVM world, and create a plugin for it just for fun.
It doesn't do any help to me in real life for now.

## Open/Build the sample LLVM plugin
* CLion / VSCode / Command Line
* CMake

## Compile other project with the plugin

### macOS

LLVM14 is installed via homebrew, maybe I should build one.

```bash
# install llvm14 using brew
brew install llvm@14

export PATH="/opt/homebrew/opt/llvm@14/bin:$PATH"

# setup CXX & CC，CMAKE 会使用这两个环境变量
export CXX=/opt/homebrew/opt/llvm@14/bin/clang++
export CC=/opt/homebrew/opt/llvm@14/bin/clang


### 创建一个 build 文件夹
mkdir cmake-build-debug && cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..

## build whenever source code changes, which will create TestLLVM01Lib.dylib under current folder
cmake --build .

## compile other cc with the plugin
## note: MUST use the same compiler as the plugin use
# back to root of this repo
cd ../
$CXX -fplugin=./cmake-build-debug/TestLLVM01Lib.dylib ./test/test_main.cpp -v
./a.out
```

### Ubuntu
llvm 15 is installed via `sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"`
```bash
clang-15 -fplugin=./build/TestLLVM01Lib.so ./test/test_main.cpp -lstdc++
```

## OptionalGuard
on macOS
```bash
# -fsyntax-only 不会编译，只是生成结果
# -Xclang -plugin -Xclang optional-guard 相当于要传给 clang frontend 这样的参数  -plugin optional-guard
$CXX -fsyntax-only -std=c++17 -fplugin=./build/OptionalGuard/OptionalGuardPlugin.dylib -Xclang -plugin -Xclang optional-guard ./test/option_test_main.cpp -o ./option_test_main -v

```
去掉  -fsyntax-only 之后，会报 链接错误，原因是前一步编译结果没出来
## clang-query

on macOS

```bash
## enter clang-query interaction env
/opt/homebrew/opt/llvm@14/bin/clang-query test/option_test_main.cpp

## set traversal mode & output the generated AST tree of matched nodes
clang-query> set traversal IgnoreUnlessSpelledInSource
clang-query> set output dump
clang-query> match functionDecl(hasName("bad_case")).bind("bad_case_function")

Match #1:

Binding for "bad_case_function":
FunctionDecl 0x1280debd8 </Users/joey/Documents/Code/TestLLVM01/test/option_test_main.cpp:20:1, line:24:1> line:20:6 used bad_case 'void ()'
`-CompoundStmt 0x1280dee70 <col:17, line:24:1>
  `-DeclStmt 0x1280dee58 <line:22:3, col:26>
    `-VarDecl 0x1280dece8 <col:3, col:25> col:7 value 'int' cinit
      `-CallExpr 0x1280dee38 <col:15, col:25> '<dependent type>' contains-errors
        `-CXXDependentScopeMemberExpr 0x1280dedf0 <col:15, col:19> '<dependent type>' contains-errors lvalue .value
          `-RecoveryExpr 0x1280dedd0 <col:15> '<dependent type>' contains-errors lvalue

Binding for "root":
FunctionDecl 0x1280debd8 </Users/joey/Documents/Code/TestLLVM01/test/option_test_main.cpp:20:1, line:24:1> line:20:6 used bad_case 'void ()'
`-CompoundStmt 0x1280dee70 <col:17, line:24:1>
  `-DeclStmt 0x1280dee58 <line:22:3, col:26>
    `-VarDecl 0x1280dece8 <col:3, col:25> col:7 value 'int' cinit
      `-CallExpr 0x1280dee38 <col:15, col:25> '<dependent type>' contains-errors
        `-CXXDependentScopeMemberExpr 0x1280dedf0 <col:15, col:19> '<dependent type>' contains-errors lvalue .value
          `-RecoveryExpr 0x1280dedd0 <col:15> '<dependent type>' contains-errors lvalue


```

See: [Exploring Clang Tooling Part 2: Examining the Clang AST with clang-query](https://devblogs.microsoft.com/cppblog/exploring-clang-tooling-part-2-examining-the-clang-ast-with-clang-query/)
