
#ifndef _AUTO_LOAD_PASS_H_
#define _AUTO_LOAD_PASS_H_
#include "llvm/IR/PassManager.h"

namespace llvm {

class Function;

struct AutoLoadAOPInjectionPass : public llvm::PassInfoMixin<AutoLoadAOPInjectionPass> {
  // run module pass and module analysis pass
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);

  std::set<Function*> GetAnnotatedFunctions(const llvm::Module &M, const std::string &AnnotationString);

  bool RunAOPInjection(llvm::Module &M, std::set<Function*> &annotFuncs);

  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }

private:
  std::set<Function*> functions_with_annotate_;
};

}
#endif  // _AUTO_LOAD_PASS_H_
