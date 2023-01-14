//
// Created by XueshiQiao on 2023/1/14.
//

#ifndef OPTIONAL_GUARD_PLUGIN_H
#define OPTIONAL_GUARD_PLUGIN_H

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"

#include <memory>

namespace my_clang_plugins {

class OptionalGuardPluginAction : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI,
                                                        llvm::StringRef) override;

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  clang::PluginASTAction::ActionType getActionType() override {
    return CmdlineAfterMainAction;
  }

  clang::ast_matchers::DeclarationMatcher buildMatcher();

private:
  std::unique_ptr<clang::ast_matchers::MatchFinder> ast_finder_;
  std::unique_ptr<clang::ast_matchers::MatchFinder::MatchCallback> match_result_callback_;
};

}

#endif  // OPTIONAL_GUARD_PLUGIN_H
