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
  for (auto func : functions) {
    std::cout << "------------RunAOPInjection on function: " << func->getName().str() << std::endl;
    for (auto bb = func->begin(); bb != func->end(); ++bb) {
      for (auto inst = bb->begin(); inst != bb->end(); ++inst) {
        if (inst->getOpcode() == Instruction::Ret) {
          // std::cout << "------------instruction: " << inst->getOpcodeName() << std::endl;
          // IRBuilder<> builder(inst);
          // builder.CreateCall(func);
        }
      }
    }
  }

  auto& context = M.getContext();
  auto printf_type =
      FunctionType::get(Type::getInt32Ty(context),
                        {Type::getInt8PtrTy(context)}, /*IsVarArgs=*/false);
  FunctionCallee callee =
      M.getOrInsertFunction("my_aop_intercepter", printf_type);

  for (auto F : functions) {
    if (F->isDeclaration()) {
      continue;
    }
    // Get an IR builder. Sets the insertion point to the top of the function
    IRBuilder<> Builder(&*F->getEntryBlock().getFirstInsertionPt());
    // Inject a global variable that contains the function name
    auto FuncName = Builder.CreateGlobalStringPtr(F->getName());
    Builder.CreateCall(callee, {FuncName});
  }

  //   // STEP 3: For each function in the module, inject a call to printf
//   // ----------------------------------------------------------------
//   for (auto &F : M) {
//     if (F.isDeclaration())
//       continue;

//     // Get an IR builder. Sets the insertion point to the top of the function
//     IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());

//     // Inject a global variable that contains the function name
//     auto FuncName = Builder.CreateGlobalStringPtr(F.getName());

//     // Printf requires i8*, but PrintfFormatStrVar is an array: [n x i8]. Add
//     // a cast: [n x i8] -> i8*
//     llvm::Value *FormatStrPtr =
//         Builder.CreatePointerCast(PrintfFormatStrVar, PrintfArgTy, "formatStr");

//     // The following is visible only if you pass -debug on the command line
//     // *and* you have an assert build.
//     LLVM_DEBUG(dbgs() << " Injecting call to printf inside " << F.getName()
//                       << "\n");

//     // Finally, inject a call to printf
//     Builder.CreateCall(
//         Printf, {FormatStrPtr, FuncName, Builder.getInt32(F.arg_size())});

//     InsertedAtLeastOnePrintf = true;
//   }

}
//   bool InsertedAtLeastOnePrintf = false;

//   auto &CTX = M.getContext();
//   PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));

//   // STEP 1: Inject the declaration of printf
//   // ----------------------------------------
//   // Create (or _get_ in cases where it's already available) the following
//   // declaration in the IR module:
//   //    declare i32 @printf(i8*, ...)
//   // It corresponds to the following C declaration:
//   //    int printf(char *, ...)
//   FunctionType *PrintfTy = FunctionType::get(
//       IntegerType::getInt32Ty(CTX),
//       PrintfArgTy,
//       /*IsVarArgs=*/true);

//   FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy);

//   // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
//   Function *PrintfF = dyn_cast<Function>(Printf.getCallee());
//   PrintfF->setDoesNotThrow();
//   PrintfF->addParamAttr(0, Attribute::NoCapture);
//   PrintfF->addParamAttr(0, Attribute::ReadOnly);


//   // STEP 2: Inject a global variable that will hold the printf format string
//   // ------------------------------------------------------------------------
//   llvm::Constant *PrintfFormatStr = llvm::ConstantDataArray::getString(
//       CTX, "(llvm-tutor) Hello from: %s\n(llvm-tutor)   number of arguments: %d\n");

//   Constant *PrintfFormatStrVar =
//       M.getOrInsertGlobal("PrintfFormatStr", PrintfFormatStr->getType());
//   dyn_cast<GlobalVariable>(PrintfFormatStrVar)->setInitializer(PrintfFormatStr);

//   // STEP 3: For each function in the module, inject a call to printf
//   // ----------------------------------------------------------------
//   for (auto &F : M) {
//     if (F.isDeclaration())
//       continue;

//     // Get an IR builder. Sets the insertion point to the top of the function
//     IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());

//     // Inject a global variable that contains the function name
//     auto FuncName = Builder.CreateGlobalStringPtr(F.getName());

//     // Printf requires i8*, but PrintfFormatStrVar is an array: [n x i8]. Add
//     // a cast: [n x i8] -> i8*
//     llvm::Value *FormatStrPtr =
//         Builder.CreatePointerCast(PrintfFormatStrVar, PrintfArgTy, "formatStr");

//     // The following is visible only if you pass -debug on the command line
//     // *and* you have an assert build.
//     LLVM_DEBUG(dbgs() << " Injecting call to printf inside " << F.getName()
//                       << "\n");

//     // Finally, inject a call to printf
//     Builder.CreateCall(
//         Printf, {FormatStrPtr, FuncName, Builder.getInt32(F.arg_size())});

//     InsertedAtLeastOnePrintf = true;
//   }

//   return InsertedAtLeastOnePrintf;
// }


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
