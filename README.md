# TestLLVMASTPlugin

This project is my attempt to explore LLVM world, and create a plugin for it just for fun.
It doesn't do any help to me in real life for now. 

## Open/Build the sample LLVM plugin
* CLion / VSCode 
* CMake

## Compile other project with the plugin

### macOS

LLVM(13) is installed via homebrew, maybe I should build one.
```bash
clang -fplugin=./cmake-build-debug/TestLLVM01Lib.dylib ./test/test_main.cpp -v
```

### Ubuntu
llvm 15 is installed via `sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"`
```bash
clang-15 -fplugin=./build/TestLLVM01Lib.so ./test/test_main.cpp -lstdc++
```

The plugin works alright, but there's an error in macOS, maybe something wrong with the `syslibroot` 

```
clang -fplugin=./cmake-build-debug/TestLLVM01Lib.dylib ./test/test_main.cpp -v
Homebrew clang version 13.0.0
Target: arm64-apple-darwin22.1.0
Thread model: posix
InstalledDir: /opt/homebrew/opt/llvm/bin
 "/opt/homebrew/Cellar/llvm/13.0.0/bin/clang-13" -cc1 -triple arm64-apple-macosx13.0.0 -Wundef-prefix=TARGET_OS_ -Werror=undef-prefix -Wdeprecated-objc-isa-usage -Werror=deprecated-objc-isa-usage -emit-obj -mrelax-all --mrelax-relocations -disable-free -disable-llvm-verifier -discard-value-names -main-file-name test_main.cpp -mrelocation-model pic -pic-level 2 -mframe-pointer=non-leaf -fno-rounding-math -munwind-tables -fcompatibility-qualified-id-block-type-checking -fvisibility-inlines-hidden-static-local-var -target-cpu apple-m1 -target-feature +v8.5a -target-feature +fp-armv8 -target-feature +neon -target-feature +crc -target-feature +crypto -target-feature +dotprod -target-feature +fp16fml -target-feature +ras -target-feature +lse -target-feature +rdm -target-feature +rcpc -target-feature +zcm -target-feature +zcz -target-feature +fullfp16 -target-feature +sha2 -target-feature +aes -target-abi darwinpcs -fallow-half-arguments-and-returns -debugger-tuning=lldb -target-linker-version 711 -v -fcoverage-compilation-dir=/Users/joey/Documents/Code/TestLLVM01 -resource-dir /opt/homebrew/Cellar/llvm/13.0.0/lib/clang/13.0.0 -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk -stdlib=libc++ -internal-isystem /opt/homebrew/opt/llvm/bin/../include/c++/v1 -internal-isystem /Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk/usr/local/include -internal-isystem /opt/homebrew/Cellar/llvm/13.0.0/lib/clang/13.0.0/include -internal-externc-isystem /Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk/usr/include -fdeprecated-macro -fdebug-compilation-dir=/Users/joey/Documents/Code/TestLLVM01 -ferror-limit 19 -stack-protector 1 -fblocks -fencode-extended-block-signature -fregister-global-dtors-with-atexit -fgnuc-version=4.2.1 -fcxx-exceptions -fexceptions -fmax-type-align=16 -fcolor-diagnostics -load ./cmake-build-debug/TestLLVM01Lib.dylib -D__GCC_HAVE_DWARF2_CFI_ASM=1 -o /var/folders/hf/70mw5sd94xxbl06_hkqrb3400000gn/T/test_main-2cde83.o -x c++ ./test/test_main.cpp
clang -cc1 version 13.0.0 based upon LLVM 13.0.0 default target arm64-apple-darwin22.1.0
ignoring nonexistent directory "/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk/usr/local/include"
ignoring nonexistent directory "/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk/usr/include"
ignoring nonexistent directory "/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk/System/Library/Frameworks"
ignoring nonexistent directory "/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk/Library/Frameworks"
#include "..." search starts here:
#include <...> search starts here:
 /opt/homebrew/opt/llvm/bin/../include/c++/v1
 /opt/homebrew/Cellar/llvm/13.0.0/lib/clang/13.0.0/include
End of search list.
========== func_name: Func1
./test/test_main.cpp:6:5: warning: Function name should start with lower case letter
int Func1() {
    ^
========== func_name: main
1 warning generated.
 "/usr/bin/ld" -demangle -lto_library /opt/homebrew/Cellar/llvm/13.0.0/lib/libLTO.dylib -no_deduplicate -dynamic -arch arm64 -platform_version macos 13.0.0 13.0.0 -syslibroot /Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk -o a.out /var/folders/hf/70mw5sd94xxbl06_hkqrb3400000gn/T/test_main-2cde83.o -lSystem /opt/homebrew/Cellar/llvm/13.0.0/lib/clang/13.0.0/lib/darwin/libclang_rt.osx.a
ld: library not found for -lSystem
clang-13: error: linker command failed with exit code 1 (use -v to see invocation)
```
