//===--- CppformatArgumentsCheck.cpp - clang-tidy--------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "CppformatArgumentsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {

void CppformatArgumentsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      callExpr(callee(expr(ignoringParenImpCasts(
                   declRefExpr(to(functionDecl(hasName("::fmt::format"))))
                       .bind("declref")))))
          .bind("call"),
      this);
}

void CppformatArgumentsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Call = Result.Nodes.getNodeAs<CallExpr>("call");
  if (!Call)
    return;

  SourceLocation BeginLoc = Call->getLocStart();
  if (BeginLoc.isInvalid())
    return;

  diag(BeginLoc, "function '%0' is insufficiently awesome")
      << Call->getName()
      << FixItHint::CreateInsertion(BeginLoc, "awesome_");
}

} // namespace tidy
} // namespace clang
