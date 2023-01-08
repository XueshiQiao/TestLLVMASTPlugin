#include <iostream>
#include "test.h"

int main() {
  auto test = std::make_shared<Test>();
  test->Print();
  return 0;
}
