#include <iostream>

__attribute__((annotate("max_annotation")))
int max(int x, int y) __attribute__((annotate("max-123"))) { return x > y ? x : y; }

__attribute__((annotate("min_annotation"))) int min(int x, int y) { return x < y ? x : y; }

__attribute__((annotate("AOPInjection", "max_annotation", "min_annotation")))
void hello(){
    std::cout << "hello" << std::endl;
}


[[clang::annotate("AOPInjection")]] void hello2(){
    std::cout << "hello2" << std::endl;
}

__attribute__((annotate("AOPInjection", "max_annotation",
                        "min_annotation"))) void
hello3();

void hello3() { std::cout << "hello3" << std::endl; }

extern "C" {

void my_aop_intercepter(char* func_name) {
  std::cout << "my_aop_intercepter" << func_name << std::endl;
}

}




int main() {
  int x = 10;
  int y = 20;
  std::cin >> x >> y;
  x = min(x, y);
  hello();
  hello2();
  hello3();
  max(x, y);
  return 0;
}
