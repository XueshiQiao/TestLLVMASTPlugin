#include "InstructionCountPass.h"

#include <iostream>
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace llvm {
  // function pass
  // It takes two arguments: a Function class that we will work on
  // and a FunctionAnalysisManager class that can give you analysis data.
  // It returns a PreservedAnalyses instance, which tells PassManager (and
  // AnalysisManager) what analysis data was invalidated by this Pass.
llvm::PreservedAnalyses
InstructionCountPass::run(llvm::Function &F,
                          llvm::FunctionAnalysisManager &FAM) {
    std::cout << "===============enter function (" << F.getName().str() << ")==============="
            << std::endl;
    int count = 0;
    // print F's all attributes
    // std::cout <<  "------------dump function attributes------------" << std::endl;
    // F.getAttributes().dump();
    // std::cout << "------------dump function attributes end------------"
              // << std::endl;
    // llvm::errs() << "------------hello world== ";
    //     llvm::errs().write_escaped(F.getName()) << '\n';

    auto attrs = F.getAttributes();
    F.addFnAttr("hello------------attr", "world");

    // F.printAsOperand(llvm::errs() << "hello world--------------");
    // llvm::errs() << "\n";

    // iterator attrs
    for (auto it = attrs.begin(); it != attrs.end(); it++) {
      std::cout << "------------attr: " << it->getAsString() << std::endl;
      F.printAsOperand(llvm::errs() << "------------attr: " << it->getAsString());
      llvm::errs() << "\n";

    }
    // F.getAttributes().getFnAttributes().dump();
    // F.getAttributes().getRetAttributes().dump();
    // F.getAttributes().getParamAttributes(0).dump();
    // F.getAttributes().getParamAttributes(1).dump();
    // F.getAttributes().getParamAttributes(2).dump();
    // F.getAttributes().getParamAttributes(3).dump();
    // F.getAttributes().getParamAttributes(4).dump();

    for (llvm::BasicBlock &BB : F) {
      for (llvm::Instruction &I : BB) {
        I.printAsOperand(llvm::errs() << "------------instruction: ");
        llvm::errs() << "\n";
        count++;
      }
    }
    std::cout << "------------Function: " << F.getName().str() << ", instruction count: " << count << std::endl;
    return llvm::PreservedAnalyses::none();
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "InstructionCountPass", "v0.1",
        [](llvm::PassBuilder &PB) {
// #if 0

          PB.registerPipelineParsingCallback(
              [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                 llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                if (Name == "instruction-count") {
                  FPM.addPass(InstructionCountPass());
                  return true;
                }
                return false;
              });

// #else

//               // Run StrictOpt before other optimizations when the optimization
//       // level is at least -O2
//       // using OptimizationLevel= typename llvm::PassBuilder::OptimizationLevel;
//       PB.registerPipelineStartEPCallback(
//         [](llvm::ModulePassManager &MPM, llvm::OptimizationLevel OL) {
//           if (OL.getSpeedupLevel() >= 2)
//             // Since `PassBuilder::registerPipelineStartEPCallback`
//             // only accept ModulePass, we need an adapter to make
//             // it work.
//             MPM.addPass(createModuleToFunctionPassAdaptor(InstructionCountPass()));
//         });

// #endif

    }
  };
}
}
