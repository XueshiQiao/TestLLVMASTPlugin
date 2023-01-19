#include <cassert>
#include <memory>
#include <iostream>
#include <string>
#include <unordered_set>

#define CLASS_ANNOTATE __attribute__((annotate("custom_class_annotation")))
#define METHOD_ANNOTATE __attribute__((annotate("custom_method_annotation")))
#define FIELD_ANNOTATE __attribute__((annotate("custom_field_annotation")))

#define EVENT_ANNOTATE(ARG) __attribute__((annotate("custom_event_annotation:" #ARG)))

class CLASS_ANNOTATE ClassWithAnnotate {
public:
  ClassWithAnnotate() = default;

  void METHOD_ANNOTATE EVENT_ANNOTATE(ENTER) Enter() {
    joined = true;
    std::cout << "JOIN, " << joined << std::endl;
  }

  void METHOD_ANNOTATE EVENT_ANNOTATE(EXIT) Exit() {
    joined = false;
    std::cout << "EXIT, " << joined << std::endl;
  }

  void RecordEvent(std::string event) {
    events_.insert(event);
    std::cout << "RECORD EVENT: " << event << std::endl;
  }

  std::unordered_set<std::string> GetEvents() {
    return events_;
  }

private:
  bool FIELD_ANNOTATE joined;
  std::unordered_set<std::string> events_;
};

int main() {
  ClassWithAnnotate class_with_annotate;
  class_with_annotate.Enter();
  class_with_annotate.Exit();

  auto events = class_with_annotate.GetEvents();
  for (auto event : events) {
    std::cout << "EVENT: " << event << std::endl;
  }
  assert(events.size() == 2);

  return 0;
}
