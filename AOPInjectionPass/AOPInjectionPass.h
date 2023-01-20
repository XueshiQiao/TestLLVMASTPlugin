
#ifndef _AOP_INJECTION_PASS_H_
#define _AOP_INJECTION_PASS_H_

#include "llvm/IR/PassManager.h"
#include <set>

namespace llvm {

struct AOPInjectionPass : public llvm::PassInfoMixin<AOPInjectionPass> {
  // run module pass and module analysis pass
  llvm::PreservedAnalyses run(const llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);

  std::set<Function*> GetAnnotatedFunctions(const llvm::Module &M, const std::string &AnnotationString);

  void RunAOPInjection(std::set<Function*> &annotFuncs);

  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }

private:
  std::set<Function*> functions_with_annotate_;
};


}
#endif  // _AOP_INJECTION_PASS_H_
