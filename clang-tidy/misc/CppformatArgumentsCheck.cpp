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

  const unsigned NumUsefulArgs = NumArgs - 1 - IgnoreNumArgs;

  const Expr *Arg = Call->getArg(IgnoreNumArgs)->IgnoreImpCasts();
  const StringLiteral *FormatStringArg = dyn_cast<StringLiteral>(Arg);
  if (!FormatStringArg)
    return;

  const StringRef FormatString = FormatStringArg->getBytes();

  // Check format string.
  bool UnspecifiedArgIndices = false;
  SmallVector<bool, 0> UsedArgs(NumUsefulArgs, /*Value=*/false);

  bool HasOpeningBrace = false;
  size_t LastOpeningBracePos = StringRef::npos;
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
      } else {
        assert(LastOpeningBracePos != StringRef::npos);
        assert(LastOpeningBracePos + 1 <= Pos);
        StringRef ArgFormat = FormatString.slice(LastOpeningBracePos + 1, Pos);

        if (ArgFormat.size() > 0) {
          size_t ClosingPos = ArgFormat.size();
          // Find colon.
          const size_t ColonPos = ArgFormat.find(':');
          if ((ColonPos != StringRef::npos) && (ColonPos > 0))
            ClosingPos = ColonPos;

          if (ClosingPos > 0) {
            // Check the argument format.
            size_t ArgNum;
            SourceLocation ArgLoc = FormatStringArg->getLocationOfByte(
                LastOpeningBracePos + 1, *SM, LangOpts, Target);
            if (ArgFormat.substr(0, ClosingPos)
                    .getAsInteger(/*Radix=*/10, ArgNum)) {
              // Wrong format, cannot parse.
              diag(ArgLoc,
                   "incorrect format string: cannot parse argument number");
            } else {
              if (ArgNum >= NumUsefulArgs) {
                // Out of bounds argument index.
                diag(ArgLoc, "incorrect format string: argument is out of "
                             "bounds (should be < %0)")
                    << NumUsefulArgs;
              } else {
                UsedArgs[ArgNum] = true;
              }
            }
          } else {
            // Unspecified index was used.
            UnspecifiedArgIndices = true;
          }
        } else {
          // Unspecified index was used.
          UnspecifiedArgIndices = true;
        }
      }

      HasOpeningBrace = false;
      LastOpeningBracePos = StringRef::npos;
    } else {
      continue;
    }
  }

  // From doc (http://cppformat.github.io/latest/syntax.html#format-string-syntax):
  // If the numerical arg_indexes in a format string are 0, 1, 2, ... in sequence,
  // they can all be omitted (not just some) and the numbers 0, 1, 2, ... will
  // be automatically inserted in that order.
  if (UnspecifiedArgIndices) {
    // Some argument indices have been unspecified...
    if (std::any_of(UsedArgs.begin(), UsedArgs.end(),
                    [](bool v) { return v; })) {
      // ...and some have been specified.
      SourceLocation ArgLoc = FormatStringArg->getLocStart();
      diag(ArgLoc, "incorrect format string: all or none of the argument "
                   "indices must be specified");
    }
  } else {
    // No argument index has been left unspecified.
    for (size_t ArgNum = 0u; ArgNum < NumUsefulArgs; ++ArgNum) {
      if (!UsedArgs[ArgNum]) {
        const Expr *Arg =
            Call->getArg(ArgNum + 1 + IgnoreNumArgs)->IgnoreImpCasts();
        assert(Arg);
        SourceLocation ArgLoc = Arg->getLocStart();
        diag(ArgLoc, "incorrect format string: unused argument");
      }
    }
  }

  // Check if closed.
  if (HasOpeningBrace) {
    SourceLocation BraceLoc = FormatStringArg->getLocationOfByte(
        LastOpeningBracePos, *SM, LangOpts, Target);
    diag(BraceLoc, "incorrect format string: unmatched opening brace")
        << FixItHint::CreateInsertion(BraceLoc.getLocWithOffset(1), "}");
  }
}

} // namespace misc
} // namespace tidy
} // namespace clang
