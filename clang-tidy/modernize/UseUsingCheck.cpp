//===--- UseUsingCheck.cpp - clang-tidy------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "UseUsingCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace modernize {

void UseUsingCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(typedefDecl().bind("typedef"), this);
}

SourceLocation backwardSkipWhitespace(SourceLocation Loc,
                                      const SourceManager &SM,
                                      const ASTContext *Context) {
  assert(Loc.isValid());
  Loc = Loc.getLocWithOffset(-1);
  assert(Loc.isValid());
  while (isWhitespace(*FullSourceLoc(Loc, SM).getCharacterData())) {
    Loc = Loc.getLocWithOffset(-1);
    assert(Loc.isValid());
  }
  return Loc.getLocWithOffset(1);
}

void UseUsingCheck::check(const MatchFinder::MatchResult &Result) {
  const ASTContext *Context = Result.Context;
  const auto LangOpts = Context->getLangOpts();
  if (!LangOpts.CPlusPlus11)
    return;

  const SourceManager &SM = *Result.SourceManager;

  const auto *Typedef = Result.Nodes.getNodeAs<TypedefDecl>("typedef");

  static const StringRef Message = "use 'using' instead of 'typedef'";
  auto BeginLoc = Typedef->getSourceRange().getBegin();
  if (BeginLoc.isInvalid())
    return;
  DiagnosticBuilder Diag = diag(BeginLoc, Message);

  const auto TypedefRange =
      SourceRange(BeginLoc, BeginLoc.getLocWithOffset(std::strlen("typedef")));
  Diag << FixItHint::CreateReplacement(TypedefRange, "using ");

  const SourceLocation NameLoc = Typedef->getLocation();
  const SourceLocation BeforeNameLoc =
      backwardSkipWhitespace(NameLoc, SM, Context);
  CharSourceRange NameRange = CharSourceRange::getTokenRange(NameLoc, NameLoc);
  CharSourceRange SpaceNameRange =
      CharSourceRange::getTokenRange(BeforeNameLoc, NameLoc);

  const std::string Name = Lexer::getSourceText(NameRange, SM, LangOpts);
  Diag << FixItHint::CreateInsertion(TypedefRange.getEnd(), Name + " =");
  Diag << FixItHint::CreateRemoval(SpaceNameRange);
}

} // namespace modernize
} // namespace tidy
} // namespace clang
