#include "AutoLoadAOPInjectionPassV2.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

namespace {
constexpr char AOPInjectionPointCutAnnotate[] = "AOPInjectionPointCut";
constexpr char InjectedAnnotation[] = "Injected";
constexpr char AOPJoinedPointCutPrefix[] = "joined_point_cut:";
}

namespace llvm {

llvm::PreservedAnalyses AutoLoadAOPInjectionPassV2::run(llvm::Module &M,
                                              llvm::ModuleAnalysisManager &MAM) {
  std::cout << "Enter module (" << M.getName().str() << ")" << std::endl;
  do {
    AnnotatedFunctions functions = GetAnnotatedFunctions(M);
    if (functions.empty()) {
      break;
    }
    auto aop_functions = ParseAOPFunctionsFromAnnotatedFunctions(functions);
    if (aop_functions.empty()) {
      break;
    }

    bool changed = false;
    for (auto &aop : aop_functions) {
      changed |= RunAOPInjection(M, aop.matched_functions, aop.point_cut_function);
    }
    if (changed) {
      return llvm::PreservedAnalyses::none();
    }
  } while (false);
  return llvm::PreservedAnalyses::all();
}

std::vector<AutoLoadAOPInjectionPassV2::AOPFunction>
AutoLoadAOPInjectionPassV2::ParseAOPFunctionsFromAnnotatedFunctions(
    AnnotatedFunctions &annotated_functions) {
  std::vector<AOPFunction> aop_functions;
  std::unordered_map<Function *, AOPFunction*> aop_function_map;
  // fill point_cut_function & annotation
  for (auto &func : annotated_functions) {
    if (func.second == AOPInjectionPointCutAnnotate) {
      AOPFunction aop_function;
      aop_function.point_cut_function = func.first;
      aop_function.annotation = func.second;
      aop_functions.push_back(aop_function);
      aop_function_map.insert({aop_function.point_cut_function, &aop_function});
      std::cout << "found point cut function: "
                << aop_function.point_cut_function->getName().str()
                << std::endl;
    }
  }

  // fill point_cut_matching_string
  for (auto &func : annotated_functions) {
    if (func.second.startswith(AOPJoinedPointCutPrefix)) {
      auto it = std::find_if(aop_functions.begin(), aop_functions.end(),
                             [&](AOPFunction &aop_function) {
        return aop_function.point_cut_function == func.first;
      });

      if (it != aop_functions.end()) {
        it->point_cut_matching_string = func.second.str().substr(strlen(AOPJoinedPointCutPrefix));
        std::cout << "found matching string: "
                  << it->point_cut_matching_string << " for function: "
                  << it->point_cut_function->getName().str()
                  << std::endl;
      }
    }
  }

  // fill functions_with_annotation
  for (auto &func : annotated_functions) {
    if (func.second == InjectedAnnotation) {
      for (auto &aop : aop_functions) {
        if (func.first->getName().contains(aop.point_cut_matching_string)) {
          aop.matched_functions.insert(func.first);
          std::cout << "add function : "
                    << func.first->getName().str()
                    << "(contains: " << aop.point_cut_matching_string << " )"
                    << " to point cut: " << aop.point_cut_function->getName().str()
                    << std::endl;
        }
      }
    }
  }
  return aop_functions;
}

AutoLoadAOPInjectionPassV2::AnnotatedFunctions
AutoLoadAOPInjectionPassV2::GetAnnotatedFunctions(const llvm::Module &M) {
  AutoLoadAOPInjectionPassV2::AnnotatedFunctions functions;
  const llvm::GlobalVariable* annotations = M.getNamedGlobal("llvm.global.annotations");
  if (annotations) {
    ConstantArray *const_array = dyn_cast<ConstantArray>(annotations->getOperand(0));
    for (auto op_it = const_array->op_begin(); op_it != const_array->op_end(); ++op_it) {
      ConstantStruct *const_struct = dyn_cast<ConstantStruct>(op_it->get());
      Function *function = dyn_cast<Function>(const_struct->getOperand(0)->getOperand(0));
      auto annotations = dyn_cast<ConstantDataArray>(dyn_cast<GlobalVariable>(const_struct->getOperand(1)->getOperand(0))->getInitializer());
      llvm::StringRef annotation = annotations->getAsCString();
      errs() << "Found annotated function " << function->getName() << " -> "
             << annotation.str() << ", "
             << "\n";
      // auto atts = function->getAttributes();
      // for (auto it = atts.begin(); it != atts.end(); ++it) {
      //   errs() << "attribute: " << it->getAsString() << "\n";
      // }
      functions.push_back({function, annotation});
    }
  }
  return functions;
}

bool AutoLoadAOPInjectionPassV2::RunAOPInjection(
    llvm::Module &M, std::set<Function *> &target_functions,
    Function *injection_function) {
  // auto& context = M.getContext();
  // auto injected_func_type =
  //     FunctionType::get(Type::getInt32Ty(context),
  //                       {Type::getInt8PtrTy(context)}, /*IsVarArgs=*/false);
  FunctionCallee callee =
      M.getOrInsertFunction(injection_function->getName(), injection_function->getFunctionType());

  bool isChanged = false;
  for (auto F : target_functions) {
    if (F->isDeclaration()) {
      continue;
    }
    isChanged = true;
    // Get an IR builder. Sets the insertion point to the top of the function
    IRBuilder<> Builder(&*F->getEntryBlock().getFirstInsertionPt());
    // Inject a global variable that contains the function name
    auto FuncName = Builder.CreateGlobalStringPtr(F->getName());
    // pass the function name as a parameter to the function
    // 这里有个隐性的要求，被注入的执行的函数参数必须只且只有一个 char* 类型的参数
    Builder.CreateCall(callee, {FuncName});
  }

  return isChanged;
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "AutoLoadAOPInjectionPass", "v2",
        [](llvm::PassBuilder &PB) {
#if 0
      PB.registerPipelineParsingCallback(
        [](llvm::StringRef Name, llvm::ModulePassManager &MPM,
            llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
          if (Name == "instruction-count") {
            MPM.addPass(AutoLoadAOPInjectionPass());
            return true;
          }
          return false;
        });
#else
      PB.registerPipelineStartEPCallback(
        [](llvm::ModulePassManager &MPM, llvm::OptimizationLevel OL) {
          if (OL.getSpeedupLevel() >= 2)
            MPM.addPass(AutoLoadAOPInjectionPassV2());
        });
#endif
    }
  };
}


}
