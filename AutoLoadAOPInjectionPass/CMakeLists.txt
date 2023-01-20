cmake_minimum_required(VERSION 3.16)
project(AutoLoadAOPInjectionPass)

set(CMAKE_CXX_STANDARD 17)


message("=== run in InstuctionCountPass")

if (APPLE)
#
#   set(LLVM_DIR "/opt/homebrew/opt/llvm/lib/cmake/llvm/")
#   set(Clang_DIR "/opt/homebrew/opt/llvm/lib/cmake/clang/")
# /opt/homebrew/opt/llvm@14
  set(LLVM_DIR "/opt/homebrew/opt/llvm@14/lib/cmake/llvm/")
  set(Clang_DIR "/opt/homebrew/opt/llvm@14/lib/cmake/clang/")
endif()

if (LINUX)
  set(LLVM_DIR "/usr/lib/llvm-15/lib/cmake/llvm")
  set(Clang_DIR "/usr/lib/llvm-15/lib/cmake/clang")
endif(LINUX)

###################################### LLVM #############################################################
# Find LLVM package，resolving the include path and library
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM LIBRARY version: ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(ChooseMSVCCRT)
include(AddLLVM)
include(HandleLLVMOptions)

find_package(Clang REQUIRED)
message(STATUS "Found Clang ${CLANG_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS}
        ${CLANG_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# By default, LLVM is not built with RTTI support, but normal C++ applications are.
# A compilation error will be thrown if there is an RTTI mismatch between your code and LLVM's libraries.
add_definitions(${LLVM_DEFINITIONS})
if (NOT ${LLVM_ENABLE_RTTI})
    # For non-MSVC compilers
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

################################## LLVM end ############################################

#--------------------------------- InstructionCountPass --------------------------------------------
set(AUTO_LOAD_PASS_SRC AutoLoadAOPInjectionPass.h AutoLoadAOPInjectionPass.cpp)
add_llvm_pass_plugin(AutoLoadAOPInjectionPass ${AUTO_LOAD_PASS_SRC})

# add_llvm_library(OptionalGuardPlugin
#         MODULE
#         ${OPTIONAL_GUARD_SRC}
#         PLUGIN_TOOL clang
#         )
