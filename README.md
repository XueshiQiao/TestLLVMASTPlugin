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
