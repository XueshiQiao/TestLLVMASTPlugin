
#include <memory>
#include <iostream>

class A : public std::enable_shared_from_this<A> {
  public:
  A() {
    std::cout << "message from A::A()" << std::endl;
    std::shared_ptr<A> p = shared_from_this();
    p->print();
  }

  void print() {
    std::cout << "message from A::print()" << std::endl;
  }
};

class B : public std::enable_shared_from_this<B> {
  public:
  B() {
    std::cout << "message from B::B()" << std::endl;
    std::shared_ptr<A> p;
    p->shared_from_this()->print();
  }
};

int main() {
  std::shared_ptr<A> p = std::make_shared<A>();
  p->print();
}
