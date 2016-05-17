//===--- UseMakeUniqueCheck.cpp - clang-tidy-------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "UseMakeUniqueCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace boost {

void UseMakeUniqueCheck::registerMatchers(MatchFinder *Finder) {
  if (!getLangOpts().CPlusPlus)
    return;

  Finder->addMatcher(callExpr(callee(namedDecl(hasName("boost::make_unique"))),
                              unless(isInTemplateInstantiation()))
                         .bind("make_unique"),
                     this);
}

void UseMakeUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Call = Result.Nodes.getNodeAs<CallExpr>("make_unique");
  assert(Call);

  auto Loc = Call->getLocStart();

  auto Diag = diag(Loc, "use std::make_unique instead of boost::make_unique");

  if (Loc.isMacroID())
    return;

  const Expr *Callee = Call->getCallee();
  // const auto *Function = Callee->getAs<FunctionTemplateDecl>();

  Diag << FixItHint::CreateReplacement(
      CharSourceRange::getCharRange(Call->getLocStart(), Callee->getLocEnd()),
      "std::make_unique");
}

} // namespace boost
} // namespace tidy
} // namespace clang
