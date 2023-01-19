//
// Created by XueshiQiao on 2023/1/14.
//

#include "OptionalGuardPlugin.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

#include <memory>
#include <iostream>

using namespace clang;
using namespace ast_matchers;

namespace my_clang_plugins {

struct OptionalGuardCallback : public ast_matchers::MatchFinder::MatchCallback {
  void run(const ast_matchers::MatchFinder::MatchResult &Result) override {
    // std::cout << "--------------name:" << "=============== match result callback get called" << std::endl;;
    if (const auto *decl = Result.Nodes.getNodeAs<FunctionDecl>("func_decl")) {

      // std::cout << "====var_decl: " << decl->getNameAsString() << std::endl;
      std::cout << "--------------name:" <<decl->getNameAsString() << std::endl;;
    }
    {
      // auto *decl = Result.Nodes.getNodeAs<CXXMemberCallExpr>("var");
      // decl->getSub;

    }
  }
};

DeclarationMatcher OptionalGuardPluginAction::buildMatcher() {
  // internal::VariadicDynCastAllOfMatcher<Decl, FunctionDecl>
  return ast_matchers::functionDecl(hasName("bad_case")).bind("func_decl");
}

std::unique_ptr<ASTConsumer>
OptionalGuardPluginAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                             llvm::StringRef file) {
  std::cout << "=========CreateASTConsumer called, file: " << file.str() << std::endl;
  ast_finder_ = std::make_unique<MatchFinder>();
  match_result_callback_ = std::make_unique<OptionalGuardCallback>();

  ast_finder_->addMatcher(
      traverse(clang::TK_IgnoreUnlessSpelledInSource,
                             buildMatcher()), match_result_callback_.get());

  // functionDecl(hasDescendant(
  //     varDecl(hasDescendant(cxxDependentScopeMemberExpr(hasMemberName("value")),
  //                           has))
          // .bind("var")));

  functionDecl(hasDescendant(varDecl(hasDescendant(cxxDependentScopeMemberExpr(hasMemberName("value"), hasAncestor(ifStmt())).bind("var")))));

  // functionDecl(hasDescendant(varDecl(hasType(asString("std::optional<int>")),
  //                                    hasDescendant(cxxDependent)))
  //                            ));

  // 1. 没有 if
  // version 1& 2
  functionDecl(hasDescendant(cxxMemberCallExpr(on(declRefExpr(hasType(asString("std::optional<int>")))), unless(hasAncestor(ifStmt())))));
  functionDecl(hasDescendant(cxxMemberCallExpr(on(declRefExpr(hasType(asString("std::optional<int>")))), unless(hasAncestor(ifStmt())))));
  // version 3.
  functionDecl(hasDescendant(cxxMemberCallExpr(on(declRefExpr(hasType(recordDecl(matchesName("std::optional"))))), unless(hasAncestor(ifStmt())))));
  // version 4
  // ref: https://clang.llvm.org/docs/LibASTMatchersReference.html
  functionDecl(hasDescendant(cxxMemberCallExpr(on(declRefExpr(hasType(recordDecl(matchesName("std::optional"))))), callee(cxxMethodDecl(hasName("value"))), unless(hasAncestor(ifStmt()))).bind("call")));

  // ast_finder_->addMatcher(functionDecl(hasName("bad_case")).bind("func_decl"), match_result_callback_.get());
  return ast_finder_->newASTConsumer();
}


} // namespace my_clang_plugins

// static FrontendPluginRegistry::Add<my_clang_plugins::OptionalGuardPluginAction>
//     X("optional-guard", "optional guard plugin");
static FrontendPluginRegistry::Add<my_clang_plugins::OptionalGuardPluginAction>
  OptionalGuardPluginActionX("optional-guard", "Prompt messages to hint branches that can be"
                         " converted to ternary operators");
