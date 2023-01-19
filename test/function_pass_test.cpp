#include <iostream>

extern "C" {
__attribute__((annotate("max_annotation")))
int max(int x, int y) __attribute__((annotate("max-123"))) { return x > y ? x : y; }

__attribute__((annotate("min_annotation"))) int min(int x, int y) { return x < y ? x : y; }

}

int main() {
  int x = 10;
  int y = 20;
  std::cin >> x >> y;
  x  = min(x, y);
  return max(x, y);
}
