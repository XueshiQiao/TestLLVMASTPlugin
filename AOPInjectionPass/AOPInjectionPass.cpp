#include "AOPInjectionPass.h"

#include <iostream>
#include <string>

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"


namespace llvm {

llvm::PreservedAnalyses AOPInjectionPass::run(llvm::Module &M,
                                              llvm::ModuleAnalysisManager &MAM) {
  std::cout << "===============enter module (" << M.getName().str()
            << ")===============" << std::endl;

  std::set<Function *> functionsWithAnnotationString =
      GetAnnotatedFunctions(M, "AOPInjection");
  RunAOPInjection(M, functionsWithAnnotationString);
  return llvm::PreservedAnalyses::all();
}


std::set<Function *>
AOPInjectionPass::GetAnnotatedFunctions(const llvm::Module &M,
                                        const std::string &AnnotationString) {
  std::set<Function *> functionsWithAnnotationString;
  const llvm::GlobalVariable* annotations = M.getNamedGlobal("llvm.global.annotations");
  if (annotations) {
    ConstantArray *const_array = dyn_cast<ConstantArray>(annotations->getOperand(0));
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
  return functionsWithAnnotationString;
}

void AOPInjectionPass::RunAOPInjection(llvm::Module &M, std::set<Function *> &functions) {
  auto& context = M.getContext();
  auto aop_func_type =
      FunctionType::get(Type::getInt32Ty(context),
                        {Type::getInt8PtrTy(context)}, /*IsVarArgs=*/false);
  FunctionCallee callee =
      M.getOrInsertFunction("my_aop_intercepter", aop_func_type);

  for (auto F : functions) {
    if (F->isDeclaration()) {
      continue;
    }
    // Get an IR builder. Sets the insertion point to the top of the function
    IRBuilder<> Builder(&*F->getEntryBlock().getFirstInsertionPt());
    // Inject a global variable that contains the function name
    auto FuncName = Builder.CreateGlobalStringPtr(F->getName());
    // pass the function name as a parameter to the function
    Builder.CreateCall(callee, {FuncName});
  }
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
