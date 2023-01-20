
#ifndef _AUTO_LOAD_PASS_H_
#define _AUTO_LOAD_PASS_H_

#include "llvm/IR/PassManager.h"

#include <unordered_map>
#include <unordered_set>

namespace llvm {

class Function;

struct AutoLoadAOPInjectionPassV2
    : public llvm::PassInfoMixin<AutoLoadAOPInjectionPassV2> {

  using AnnotatedFunctions = std::vector<std::pair<Function*, llvm::StringRef>>;// std::unordered_map<std::string, std::unordered_set<Function*>>;

  struct AOPFunction {
    Function *point_cut_function = nullptr;  // 切面函数，要注入的函数
    std::string annotation;
    std::string point_cut_matching_string;  // 该切面函数的匹配字符串，其他函数名有 Injected annotation，且包含这个字符串就会被注入
    std::set<Function*> matched_functions{};  // 符合上面条件的函数集
  };

  // run module pass and module analysis pass
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);

  // 获得所有包含 annotation 的函数
  AnnotatedFunctions GetAnnotatedFunctions(const llvm::Module &M);

  // 从所有包含 annotation 的函数中，解析并构造出切面函数对象
  std::vector<AOPFunction> ParseAOPFunctionsFromAnnotatedFunctions(AnnotatedFunctions &annotatedFunctions);

  // 在每一个 target_functions 函数开始执行之前，注入执行 injection_function 函数
  bool RunAOPInjection(llvm::Module &M, std::set<Function*> &target_functions, Function* injection_function);

  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }

private:
};

}
#endif  // _AUTO_LOAD_PASS_H_
