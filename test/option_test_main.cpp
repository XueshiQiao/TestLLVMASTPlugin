#include <optional>
// #include <iostream>

void good_case1() {
  std::optional<int> opt;
  //未写完，补充hasCondition
  // functionDecl(hasDescendant(varDecl(hasDescendant(cxxDependentScopeMemberExpr(hasMemberName("value"),
  // unless(hasAncestor(ifStmt(hasCondition()))) ).bind("var")))))

  if (opt.has_value() != false) {
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

// functionDecl(hasName("bad_case"))
void bad_case() {
  std::optional<int> opt;

  // 匹配没有被 if 包着的 xx.value 的调用
  // functionDecl(hasDescendant(varDecl(hasDescendant(cxxDependentScopeMemberExpr(hasMemberName("value"), unless(hasAncestor(ifStmt())) ).bind("var")))))
  int value = opt.value();
  // std::cout << value << std::endl;

  // clang query 获取 if 中没有判断是否为空的 xx.value 的调用
  // functionDecl(hasDescendant(varDecl(hasDescendant(cxxDependentScopeMemberExpr(hasMemberName("value"), hasAncestor(ifStmt()), unless(hasAncestor(binaryOperator(hasOperatorName("!="))))).bind("var")))))
}

void other_case() {
  std::optional<bool> opt;
  if (opt.has_value()) {
    // std::cout << value << std::endl;
  }
}

class ABC {};

template <typename T> class Test {

};

std::optional<int> opt_test;
std::optional<bool> opt_test_bool;
Test<int> test_int;


enum Kind {
  KindA,
  KingB,
  KindC,
  KindD,
};

void testSwitchKind() {
  Kind a = KindA;
  switch(a) {
    case KindA:
    case KingB:
    case KindC:
      return;
  }
  return ;
}

// m recordDecl(hasDescendant(fieldDecl(hasType(recordDecl(hasName("Rectangle")))).bind("field"))).bind("class")
class Rectangle {
public:
  Rectangle(int width, int height) : width_(width), height_(height) {}
private:
  int width_;
  int height_;
};

// 找到哪些类中有 Rectangle 类型的成员
// m recordDecl(hasDescendant(fieldDecl(hasType(recordDecl(hasName("Rectangle")))).bind("field"))).bind("class")
class Cube {
public:
  Cube(int width, int height, int depth)
      : rectangle_(width, height), depth_(depth) {}
private:
  Rectangle rectangle_;
  int depth_;
};



int main() {
  good_case1();
  good_case2();
  bad_case();

  // varDecl(hasType(asString("Test<int>")))
  return 0;
}
