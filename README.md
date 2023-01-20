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

## AutoLoadAOPInjectionPass
上面的 AOPInjectionPass 需要手动传入 --passes="aop-injection" 参数来启动该 pass，另外一种方式是默认加载，区别是需要通过 `PB.registerPipelineStartEPCallback()` 来注册 pass

还有好处是，可以直接通过 clang 动态链接 pass plugin 的动态库：

```bash
clang -lc++ -std=c++14 -O3  -Xclang -fpass-plugin="/Users/joey/Documents/Code/TestLLVM01/build/AutoLoadAOPInjectionPass/AutoLoadAOPInjectionPass.dylib" ./test/function_pass_test.cpp -o function_pass_test

# 然后直接执行即可：
./function_pass_test

1
2
my_aop_intercepter_Z5hellov
hello
my_aop_intercepter_Z6hello2v
hello2
my_aop_intercepter_Z6hello3v
hello3
```


## AutoLoadAOPInjectionPassV2
AutoLoadAOPInjectionPassV2 实现的基本的 AOP 功能，支持通过 attribute 注入函数调用

```c++
// AOPInjectionPointCut 表示该函数可以被其他函数注入进去
// joined_point_cut:file 表示函数中包含 file 的将会在函数执行开始的时候注入调用 joinedpoint_file_related 函数
// 还需要满足另外一个条件是需要带有 Injected annotate 的函数才会被注入
__attribute__((annotate("AOPInjectionPointCut")))
__attribute__((annotate("joined_point_cut:file")))
void joinedpoint_file_related(char *func_name){
  std::cout << "joinedpoint_file_related in " << func_name << std::endl;
}

// 函数中包含 file，且有 Injected annotation，因为会被注入 joinedpoint_file_related
__attribute__((annotate("Injected"))) void file_read();
void file_read() { std::cout << "file_read, should be injected." << std::endl; }

//  >= C++11
// [[clang::annotate("Injected")]] 是 C++11 之后的写法，也会被注入
[[clang::annotate("Injected")]] void file_close(){
    std::cout << "file_close, should be injected." << std::endl;
}

// 没有 Injected 属性，即便函数名中包含 file，也不会被注入
void file_write(){
    std::cout << "file_close, should be injected." << std::endl;
}


```

How to play?

```bash
cd build/; cmake --build .; cd -

clang -lc++ -std=c++11 -O3  -Xclang -fpass-plugin="/Users/joey/Documents/Code/TestLLVM01/build/AutoLoadAOPInjectionPassV2/AutoLoadAOPInjectionPassV2.dylib" ./test/input_auto_aop_pass_v2.cpp -o input_auto_aop_pass_v2

./input_auto_aop_pass_v2

joinedpoint_file_related in _Z9file_readv
file_read, should be injected.
joinedpoint_file_related in _Z10file_closev
file_close, should be injected.
file_write, should NOT be injected.
joinedpoint_statistics in _Z18statistics_networkv
statistics_network, should be injected.
other func, should NOT be injected.
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
