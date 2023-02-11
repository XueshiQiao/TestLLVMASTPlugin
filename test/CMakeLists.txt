cmake_minimum_required(VERSION 3.16)
project(TestLLVM01)

set(CMAKE_CXX_STANDARD 17)

add_executable(OptionTestMain option_test_main.cpp )
add_executable(SharedFromThisInCons input_shared_from_this_in_cons.cpp )
#target_link_libraries(TestLLVM01 PUBLIC TestLLVM01Lib)
