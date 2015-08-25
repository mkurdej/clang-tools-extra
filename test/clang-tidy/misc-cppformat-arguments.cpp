// RUN: %python %S/check_clang_tidy.py %s misc-cppformat-arguments %t -- \
// RUN:   -std=c++11 -I %S/Inputs/misc-cppformat-arguments

#include "format.h"

// CHECK-FIXES-NOT: for ({{.*[^:]:[^:].*}})

void test_format() {
  fmt::format("no arguments");
  fmt::format("one empty argument {}", 0);
  fmt::format("one numbered argument {0}", 0);
  fmt::format("many {} empty {} arguments {} {}", 0, "1", 2, "3");
  fmt::format("many {0} positional {3} arguments {2} {1} {0}", 0, "1", 2, "3");
  // CHECK-NOT: warning:

  fmt::format(L"no arguments");
  // CHECK-NOT: warning:

  {
    const char *format_string = "format_string";
    fmt::format(format_string);
  }
  //{
  //  std::string format_string = "format_string";
  //  fmt::format(format_string);
  //}
  // CHECK-NOT: warning:

  fmt::format("unclosed single brace {", 0);
  // CHECK-MESSAGES: :[[@LINE-1]]:38: warning: incorrect format string: unclosed curly brace [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unclosed single brace {}", 0);{{$}}
  fmt::format("unclosed one of many braces { {}", 0, 1);
  // CHECK-MESSAGES: :[[@LINE-1]]:44: warning: incorrect format string: unclosed curly brace [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unclosed one of many braces {} {}", 0, 1);{{$}}
  fmt::format("unclosed many braces { { {", 0, 1, 2);
  // CHECK-MESSAGES: :[[@LINE-1]]:37: warning: incorrect format string: unclosed curly brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-2]]:39: warning: incorrect format string: unclosed curly brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-3]]:41: warning: incorrect format string: unclosed curly brace [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unclosed many braces {} {} {}", 0, 1, 2);{{$}}

  fmt::format("index out of bounds {1}", 0);
  // CHECK-MESSAGES: :[[@LINE-1]]:36: warning: incorrect format string: argument '{1}' is out of bounds [misc-cppformat-arguments]
  fmt::format("unused arguments {1} {2} {1}", 0, 1, 2);
  // CHECK-MESSAGES: :[[@LINE-1]]:47: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  fmt::format("partially specified positions {1} {} {0}", 0, 1, 2);
  // CHECK-MESSAGES: :[[@LINE-1]]:50: warning: incorrect format string: all or none of the argument indices must be specified [misc-cppformat-arguments]
  // From doc (http://cppformat.github.io/latest/syntax.html#format-string-syntax):
  //    If the numerical arg_indexes in a format string are 0, 1, 2, ... in sequence,
  //    they can all be omitted (not just some)
  //    and the numbers 0, 1, 2, ... will be automatically inserted in that order.
}

void test_format_using() {
  using fmt::format;
  format("no arguments");
  // CHECK-NOT: warning:
}
