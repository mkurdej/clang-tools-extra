//===--- CVQualifiersOrder.h - clang-tidy -----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_CV_QUALIFIERS_ORDER_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_CV_QUALIFIERS_ORDER_H

#include "../ClangTidy.h"

namespace llvm {
namespace yaml {
    template <typename T>
    struct MappingTraits;
} // namespace yaml
} // namespace llvm

namespace clang {
namespace tidy {

/// \brief Order (before/after type) of 'const' ('restrict' and 'volatile'?) qualifiers
class QualifiersOrder: public ClangTidyCheck {
public:
  QualifiersOrder(StringRef Name, ClangTidyContext *Context);
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

public:
  /// Defines the placement/alignment of CVR qualifiers
  enum /*class*/ QualifierAlignmentStyle {
    /// Leave qualifiers as they are (useful if only sorting is needed)
    // TODO(mkurdej): implement qualifier sorting
    QAS_None,
    /// Put qualifiers on the left side of (before) the type
    QAS_Left,
    /// Put qualifiers on the right side of (behind) the type
    QAS_Right
  };

private:
  void checkQualifiers(const SourceManager &SM, const ASTContext *Context,
                       TypeLoc TL, SourceRange R);

private:
  friend struct llvm::yaml::MappingTraits<QualifiersOrder>;
  QualifierAlignmentStyle QualifierAlignment;
};

} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_CV_QUALIFIERS_ORDER_H
