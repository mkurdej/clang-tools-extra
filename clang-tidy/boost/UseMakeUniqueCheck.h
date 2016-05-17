//===--- UseMakeUniqueCheck.h - clang-tidy-----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BOOST_USE_MAKE_UNIQUE_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BOOST_USE_MAKE_UNIQUE_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace boost {

/// Finds calls to ``boost::make_unique`` and replaces them with
/// ``std::make_unique`` calls.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/boost-use-make-unique.html
class UseMakeUniqueCheck : public ClangTidyCheck {
public:
  UseMakeUniqueCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace boost
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BOOST_USE_MAKE_UNIQUE_H
