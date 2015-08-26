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

  fmt::format("escaped opening brace {{");
  fmt::format("escaped closing brace }}");
  fmt::format("escaped braces {{ {} }}", 0);
  // CHECK-NOT: warning:

  const char *format_string = "format_string";
  fmt::format(format_string);
  // CHECK-NOT: warning:

  fmt::format("unmatched opening brace { not at end", 0);
  // CHECK-MESSAGES: :[[@LINE-1]]:40: warning: incorrect format string: unmatched opening brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-2]]:55: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unmatched opening brace {} not at end", 0);{{$}}
  fmt::format("unmatched opening brace at end {", 0);
  // CHECK-MESSAGES: :[[@LINE-1]]:47: warning: incorrect format string: unmatched opening brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-2]]:51: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unmatched opening brace at end {}", 0);{{$}}
  fmt::format("unmatched one of many braces { {}", 0, 1);
  // CHECK-MESSAGES: :[[@LINE-1]]:45: warning: incorrect format string: unmatched opening brace [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unmatched one of many braces {} {}", 0, 1);{{$}}
  fmt::format("unmatched many braces { { {", 0, 1, 2);
  // CHECK-MESSAGES: :[[@LINE-1]]:38: warning: incorrect format string: unmatched opening brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-2]]:40: warning: incorrect format string: unmatched opening brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-3]]:42: warning: incorrect format string: unmatched opening brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-4]]:46: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-5]]:49: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-6]]:52: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unmatched many braces {} {} {}", 0, 1, 2);{{$}}

  fmt::format("unmatched closing brace } not at end", 0);
  // CHECK-MESSAGES: :[[@LINE-1]]:40: warning: incorrect format string: unmatched closing brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-2]]:55: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unmatched closing brace {} not at end", 0);{{$}}
  fmt::format("unmatched closing brace at end }", 0);
  // CHECK-MESSAGES: :[[@LINE-1]]:47: warning: incorrect format string: unmatched closing brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-2]]:51: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  // CHECK-FIXES: fmt::format("unmatched closing brace at end {}", 0);{{$}}

  fmt::format("index out of bounds {1}", 0);
  // CHECK-MESSAGES: :[[@LINE-1]]:37: warning: incorrect format string: argument is out of bounds (should be < 1) [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-2]]:42: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  fmt::format("unused arguments {1} {2} {1}", 0, 1, 2);
  // CHECK-MESSAGES: :[[@LINE-1]]:47: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  fmt::format("unused arguments {1:b} {2:B} {1:d}", 0, 1, 2);
  // CHECK-MESSAGES: :[[@LINE-1]]:53: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  fmt::format("partially specified positions {1} {} {0}", 0, 1, 2);
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: incorrect format string: all or none of the argument indices must be specified [misc-cppformat-arguments]
  fmt::format("partially specified positions {1:o} {:x} {0:X}", 0, 1, 2);
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: incorrect format string: all or none of the argument indices must be specified [misc-cppformat-arguments]
}

void test_format_using() {
  using fmt::format;

  format("unmatched opening brace { not at end", 0);
  // CHECK-MESSAGES: :[[@LINE-1]]:35: warning: incorrect format string: unmatched opening brace [misc-cppformat-arguments]
  // CHECK-MESSAGES: :[[@LINE-2]]:50: warning: incorrect format string: unused argument [misc-cppformat-arguments]
  // CHECK-FIXES: format("unmatched opening brace {} not at end", 0);{{$}}

  format("no arguments");
  // CHECK-NOT: warning:
}
