
#ifndef _INSTRUCTION_COUNT_PASS_H_
#define _INSTRUCTION_COUNT_PASS_H_
#include "llvm/IR/PassManager.h"

namespace llvm {

class Function;

struct InstructionCountPass : public llvm::PassInfoMixin<InstructionCountPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};


}
#endif  // _INSTRUCTION_COUNT_PASS_H_
