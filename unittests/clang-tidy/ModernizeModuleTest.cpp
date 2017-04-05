#include "ClangTidyTest.h"
#include "modernize/UseSwapCheck.h"
#include "gtest/gtest.h"

namespace clang {
namespace tidy {
namespace test {

using modernize::UseSwapCheck;

TEST(ArgumentCommentCheckTest, CorrectComments) {
  EXPECT_NO_CHANGES(UseSwapCheck,
                    "void f(int x, int y); void g() { f(/*x=*/0, /*y=*/0); }");
  EXPECT_NO_CHANGES(UseSwapCheck,
                    "struct C { C(int x, int y); }; C c(/*x=*/0, /*y=*/0);");
}

} // namespace test
} // namespace tidy
} // namespace clang
