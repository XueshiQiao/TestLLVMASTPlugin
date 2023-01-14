#include <optional>
// #include <iostream>

void good_case1() {
  std::optional<int> opt;
  if (opt.has_value()) {
    int value = opt.value();
    // std::cout << value << std::endl;
  }
}

void good_case2() {
  std::optional<int> opt;
  if (opt) {
    int value = opt.value();
    // std::cout << value << std::endl;
  }
}

void bad_case() {
  std::optional<int> opt;
  int value = opt.value();
  // std::cout << value << std::endl;
}

int main() {
  good_case1();
  good_case2();
  bad_case();
  return 0;
}
