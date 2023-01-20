
#ifndef _INSTRUCTION_COUNT_PASS_H_
#define _INSTRUCTION_COUNT_PASS_H_
#include "llvm/IR/PassManager.h"

namespace llvm {

class Function;

struct InstructionCountPass : public llvm::PassInfoMixin<InstructionCountPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }

};

}
#endif  // _INSTRUCTION_COUNT_PASS_H_
