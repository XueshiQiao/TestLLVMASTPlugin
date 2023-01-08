cmake_minimum_required(VERSION 3.23)
project(TestLLVM01)

set(CMAKE_CXX_STANDARD 17)

# llvm is installed via brew, so it's in /opt/homebrew/opt/llvm
# 这个也可以通过 -DLLVM_DIR=XX 的方式传给 cmake 命令
set(LLVM_DIR "/opt/homebrew/opt/llvm/lib/cmake/llvm/")
set(Clang_DIR "/opt/homebrew/opt/llvm/lib/cmake/clang/")

###################################### LLVM #############################################################
# Find LLVM package，resolving the include path and library
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(ChooseMSVCCRT)
include(AddLLVM)
include(HandleLLVMOptions)

find_package(Clang REQUIRED)
message(STATUS "Found Clang ${CLANG_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS}
        ${CLANG_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# LLVM's custom CMake functions (for example, add_ llvm_executable) will be really useful here.
# But first, CMake needs to be able to find those functions.
# The following snippet imports LLVM's CMake module (more specifically, the AddLLVM CMake module),
# which contains those LLVM-specific functions/macros

set(LLVM_LINK_COMPONENTS
        Core
        Support
        Analysis
        )
##

# By default, LLVM is not built with RTTI support, but normal C++ applications are.
# A compilation error will be thrown if there is an RTTI mismatch between your code and LLVM's libraries.
add_definitions(${LLVM_DEFINITIONS})
if (NOT ${LLVM_ENABLE_RTTI})
    # For non-MSVC compilers
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

################################## LLVM end ############################################

set(SOURCE_FILES main.cpp test.cpp test.h)
set(TEST_LLVM_LIB_SOURCES main.cpp test.cpp test.h NamingPlugin.cpp NamingPlugin.h)

# Add a library target
#add_llvm_library(TestLLVM01Lib
#        MODULE
#        ${TEST_LLVM_LIB_SOURCES}
#        LINK_COMPONENTS
#        Support Analysis
#        )

add_llvm_library(TestLLVM01Lib
        MODULE
        ${TEST_LLVM_LIB_SOURCES}
        LINK_COMPONENTS
        Support Analysis
        )

## add a executable target which depends on the library TestLLVM01Lib
#add_executable(TestLLVM01 main.cpp )
#target_link_libraries(TestLLVM01 PUBLIC TestLLVM01Lib)
