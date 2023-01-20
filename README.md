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

## InstructionCountPass
compile test file function_pass_test.cpp to LLVM .ll file  or bitcode file
note: must use -O1 or upper
```bash

# use -S to emit a llvm .ll file
clang -std=c++14 -O1 -emit-llvm ./test/function_pass_test.cpp -S -o function_pass_test.ll
# use -c to emit a bitcode .bc file
clang -std=c++14 -O1 -emit-llvm ./test/function_pass_test.cpp -c -o function_pass_test.bc

```

compile our pass as InstructionCountPass.dylib
```bash
cd build
cmake --build .
```

run opt with custom pass
```bash
# using function_pass_test.cc works well too
/opt/homebrew/opt/llvm@14/bin/opt --load-pass-plugin=/Users/joey/Documents/Code/TestLLVM01/build/InstructionCountPass/InstructionCountPass.dylib --passes="function(instruction-count)" -S -o - function_pass_test.ll
```

## AOPInjectionPass
compile our pass as AOPInjectionPass.dylib
```bash
cd build
cmake --build .

# cd back to root folder

# generate llvm IR
clang -std=c++14 -emit-llvm ./test/function_pass_test.cpp -S -o function_pass_test.ll

# run opt with custom pass AOPInjectionPass
# input file: function_pass_test.ll
# output file processed by custom pass: function_pass_test_after_pass.ll
/opt/homebrew/opt/llvm@14/bin/opt --load-pass-plugin=/Users/joey/Documents/Code/TestLLVM01/build/AOPInjectionPass/AOPInjectionPass.dylib --passes="aop-injection" --disable-output -S function_pass_test.ll -o function_pass_test_after_pass.ll

# compile ll file to object file
llc -filetype=obj function_pass_test_after_pass.ll -o function_pass_test_after_pass.o
# generate executable file with clang
clang -lc++ function_pass_test_after_pass.o -o function_pass_test_after_pass

# final step, let's run it:
./function_pass_test_after_pass
```

## clang-query

on macOS

```bash
## enter clang-query interaction env

# genereate compile_commands.json file
cd some_build_folder
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .

# -p sepecify the compile_commands.json file generated in preceding step
/opt/homebrew/opt/llvm@14/bin/clang-query  test/option_test_main.cpp -p build/compile_commands.json

## set traversal mode & output the generated AST tree of matched nodes
clang-query> set traversal IgnoreUnlessSpelledInSource
clang-query> set output dump
# clang-query> set bind-root false
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
