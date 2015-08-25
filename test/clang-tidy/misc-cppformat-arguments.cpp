// RUN: %python %S/check_clang_tidy.py %s misc-cppformat-arguments %t -- \
// RUN:   -std=c++11 -I %S/Inputs/misc-cppformat-arguments

#include "format.h"

// CHECK-FIXES-NOT: for ({{.*[^:]:[^:].*}})

void f() {
  fmt::format("no arguments");
  fmt::format("one empty argument {}", 0);
  fmt::format("one numbered argument {0}", 0);
  fmt::format("many {} empty {} arguments {} {}", 0, "1", 2, "3");
  fmt::format("many {0} positional {3} arguments {2} {1} {0}", 0, "1", 2, "3");
  // CHECK-NOT: warning:

  // FIXME
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: function 'f' is insufficiently awesome [misc-cppformat-arguments]
  // CHECK-FIXES: {{^}}void awesome_f();{{$}}
}
