//
// Created by XueshiQiao on 2023/1/8.
//

#ifndef TESTLLVM01_NAMINGPLUGIN_H
#define TESTLLVM01_NAMINGPLUGIN_H

#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

namespace {
class NamingASTConsumer : public ASTConsumer {
  CompilerInstance &CI;

public:
  NamingASTConsumer(CompilerInstance &CI) : CI(CI) {}

  bool HandleTopLevelDecl(clang::DeclGroupRef DG) override {
    for (DeclGroupRef::iterator it = DG.begin(), end = DG.end(); it != end; it++) {
      const Decl *decl = *it;
      if (const FunctionDecl *func_decl = dyn_cast<FunctionDecl>(decl)) {
        std::string name = func_decl->getNameInfo().getName().getAsString();
        char &first = name.at(0);
        std::cout << "====func_name: " << name << std::endl;
        if (!(first >= 'a' && first <= 'z')) {
          CI.getDiagnostics().Report(func_decl->getLocation(),
                                     CI.getDiagnostics().getCustomDiagID(
                                         DiagnosticsEngine::Warning,
                                         "Function name should start with lower "
                                         "case letter"));
        }
      }
    }
//    for (DeclGroupRef::iterator b = D.begin(), e = D.end(); b != e; ++b) {
//      if (FunctionDecl *FD = dyn_cast<FunctionDecl>(*b)) {
//        if (FD->hasBody()) {
//          std::string name = FD->getNameInfo().getName().getAsString();
//        }
//      }
//    }
    return true;
  }

};

class PluginNamingAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<NamingASTConsumer>(CI);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  PluginASTAction::ActionType getActionType() override {
    return AddBeforeMainAction;
  }
};

static FrontendPluginRegistry::Add<PluginNamingAction> X("naming-plugin", "naming plugin");

}

#endif //TESTLLVM01_NAMINGPLUGIN_H
