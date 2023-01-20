#include <iostream>

__attribute__((annotate("AOPInjectionPointCut")))
__attribute__((annotate("joined_point_cut:file")))
void joinedpoint_file_related(char *func_name){
  std::cout << "joinedpoint_file_related in " << func_name << std::endl;
}


__attribute__((annotate("AOPInjectionPointCut")))
__attribute__((annotate("joined_point_cut:statistics")))
void joinedpoint_statistics(char *func_name){
  std::cout << "joinedpoint_statistics in " << func_name << std::endl;
}

//
__attribute__((annotate("Injected"))) void file_read();
void file_read() { std::cout << "file_read, should be injected." << std::endl; }

//  >= C++17
[[clang::annotate("Injected")]] void file_close(){
    std::cout << "file_close, should be injected." << std::endl;
}

// without attribute
void file_write(){
    std::cout << "file_write, should NOT be injected." << std::endl;
}

__attribute__((annotate("Injected"))) void statistics_network() {
  std::cout << "statistics_network, should be injected." << std::endl;
}

void other() {
    std::cout << "other func, should NOT be injected." << std::endl;
}

int main() {
  file_read();
  file_close();
  file_write();
  statistics_network();
  other();
  return 0;
}
