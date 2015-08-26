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
namespace misc {

void CppformatArgumentsCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: same for print, print_colored
  Finder->addMatcher(
      callExpr(callee(expr(ignoringParenImpCasts(
                   declRefExpr(to(functionDecl(hasName("::fmt::format"))))
                       .bind("declref")))))
          .bind("call"),
      this);
}

// Checks if the character Text[Pos] is escaped by doubling it.
bool isEscaped(StringRef Text, size_t Pos) {
  const size_t Orig = Pos;
  bool Escaped = false;
  while (Pos-- > 0u) {
    if (Text[Pos] == Text[Orig])
      Escaped = !Escaped;
    else
      break;
  }
  return Escaped;
}

void CppformatArgumentsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Call = Result.Nodes.getNodeAs<CallExpr>("call");
  const auto *DeclRef = Result.Nodes.getNodeAs<DeclRefExpr>("declref");
  assert(Call);
  assert(DeclRef);

  const SourceManager *SM = Result.SourceManager;
  const LangOptions &LangOpts = Result.Context->getLangOpts();
  const TargetInfo &Target = Result.Context->getTargetInfo();

  SourceLocation BeginLoc = Call->getLocStart();
  if (BeginLoc.isInvalid())
    return;

  unsigned IgnoreNumArgs = 0;
  // FIXME(marek): skip arguments up to the CStringRef/WStringRef
  // 0 for format
  // 0 or 1 for print (can be void/FILE*/ostream&)
  // 1 for print_colored
  unsigned NumArgs = Call->getNumArgs();
  
  if (NumArgs < IgnoreNumArgs)
    return;

  const Expr *Arg = Call->getArg(IgnoreNumArgs)->IgnoreImpCasts();
  const StringLiteral *FormatStringArg = dyn_cast<StringLiteral>(Arg);
  if (!FormatStringArg)
    return;

  const StringRef FormatString = FormatStringArg->getBytes();

  // Check format string.
  size_t NumArgsInString;
  SmallVector<size_t, 3> ArgsInString;

  size_t LastOpeningBracePos = -1;
  bool HasOpeningBrace = false;
  for (size_t Pos = 0; Pos < FormatString.size(); ++Pos) {
    if (FormatString[Pos] == '{') {
      if ((FormatString.size() > (Pos + 1)) && (FormatString[Pos + 1] == '{')) {
        // Escaped character, skip and continue.
        ++Pos;
        continue;
      }

      if (HasOpeningBrace) {
        SourceLocation BraceLoc = FormatStringArg->getLocationOfByte(
            LastOpeningBracePos, *SM, LangOpts, Target);
        diag(BraceLoc, "incorrect format string: unmatched opening brace")
            << FixItHint::CreateInsertion(BraceLoc.getLocWithOffset(1), "}");
      }

      LastOpeningBracePos = Pos;
      HasOpeningBrace = true;
    } else if (FormatString[Pos] == '}') {
      if ((FormatString.size() > (Pos + 1)) && (FormatString[Pos + 1] == '}')) {
        // Escaped character, skip and continue.
        ++Pos;
        continue;
      }

      if (!HasOpeningBrace) {
        SourceLocation BraceLoc =
            FormatStringArg->getLocationOfByte(Pos, *SM, LangOpts, Target);
        diag(BraceLoc, "incorrect format string: unmatched closing brace")
            << FixItHint::CreateInsertion(BraceLoc, "{");
      }

      HasOpeningBrace = false;
    } else {
      continue;
    }

    // Check if closed.
  }

  if (HasOpeningBrace) {
    SourceLocation BraceLoc = FormatStringArg->getLocationOfByte(
        LastOpeningBracePos, *SM, LangOpts, Target);
    diag(BraceLoc, "incorrect format string: unmatched opening brace")
        << FixItHint::CreateInsertion(BraceLoc.getLocWithOffset(1), "}");
  }

  //size_t Pos = FormatString.find('{', /*From=*/0u);
  //while (Pos != StringRef::npos) {
  //  size_t EndPos = FormatString.find('}', /*From=*/Pos + 1u);
  //  size_t OldPos = Pos;
  //  // Pos = FormatString.find('{', /*From=*/Pos + 1u);

  //  // Check if the '{' was escaped.
  //  if (isEscaped(FormatString, OldPos))
  //    continue;

  //  if ((EndPos == StringRef::npos) ||
  //      ((Pos != StringRef::npos) && (Pos < EndPos))) {
  //    SourceLocation OpeningBraceLoc =
  //        FormatStringArg->getLocationOfByte(OldPos, *SM, LangOpts, Target);
  //    diag(OpeningBraceLoc, "incorrect format string: unmatched opening brace")
  //        << FixItHint::CreateInsertion(OpeningBraceLoc.getLocWithOffset(1),
  //                                      "}");
  //  }
  //}

  //diag(BeginLoc, "function has '%0' arguments and is insufficiently awesome: "
  //               "fmt string '%1'")
  //    << NumArgs << FormatString
  //    << FixItHint::CreateInsertion(BeginLoc, "awesome_");
}

} // namespace misc
} // namespace tidy
} // namespace clang
