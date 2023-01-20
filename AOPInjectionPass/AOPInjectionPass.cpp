#include "AOPInjectionPass.h"

#include <iostream>
#include <string>

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"


namespace llvm {

llvm::PreservedAnalyses AOPInjectionPass::run(const llvm::Module &M,
                                              llvm::ModuleAnalysisManager &MAM) {
  std::cout << "===============enter module (" << M.getName().str()
            << ")===============" << std::endl;

  std::set<Function *> functionsWithAnnotationString =
      GetAnnotatedFunctions(M, "AOPInjection");
  RunAOPInjection(functionsWithAnnotationString);
  return llvm::PreservedAnalyses::all();
}


std::set<Function *>
AOPInjectionPass::GetAnnotatedFunctions(const llvm::Module &M,
                                        const std::string &AnnotationString) {
  std::set<Function *> functionsWithAnnotationString;

  auto const_global_it = M.global_begin(), const_global_end = M.global_end();
  for (; const_global_it != const_global_end; ++const_global_it) {
    std::cout << "------------global: " << const_global_it->getName().str() << std::endl;
    if (const_global_it->getName() == "llvm.global.annotations") {
      ConstantArray *const_array = dyn_cast<ConstantArray>(const_global_it->getOperand(0));
      for (auto op_it = const_array->op_begin(); op_it != const_array->op_end(); ++op_it) {
        ConstantStruct *const_struct = dyn_cast<ConstantStruct>(op_it->get());
        Function *function = dyn_cast<Function>(const_struct->getOperand(0)->getOperand(0));
        auto annotations = dyn_cast<ConstantDataArray>(dyn_cast<GlobalVariable>(const_struct->getOperand(1)->getOperand(0))->getInitializer());
        std::string annotation = annotations->getAsCString().str();
        errs() << "matching: Found annotated function " << function->getName() << " -> " << annotation << "\n";

        if(annotation.compare(AnnotationString)==0){
            functionsWithAnnotationString.insert(function);
            errs() << "MATCHED!! Found annotated function " << function->getName() << " -> " << annotation << "\n";
        }
      }
    }
  }

  return functionsWithAnnotationString;
}

void AOPInjectionPass::RunAOPInjection(std::set<Function *> &annotFuncs) {

}

// register module pass
extern "C" LLVM_ATTRIBUTE_WEAK
::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "AOPInjectionPass", "v0.1",
        [](llvm::PassBuilder &PB) {
          PB.registerPipelineParsingCallback(
              [](llvm::StringRef Name, llvm::ModulePassManager &MPM,  // <<-- using ModulePassManager here
                 llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                if (Name == "aop-injection") {
                  MPM.addPass(AOPInjectionPass());
                  return true;
                }
                return false;
              });
        }
  };
}

}  // namespace llvm
