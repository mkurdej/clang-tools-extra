// RUN: %check_clang_tidy %s readability-misleading-indentation %t -- -- -std=c++17

void foo1();
void foo2();

void fail_if_constexpr() {
  if constexpr (true) {
  }
    else {
  }
  // CHECK-MESSAGES: :[[@LINE-2]]:5: warning: different indentation for 'if' and corresponding 'else' [readability-misleading-indentation]
}

void correct_if_constexpr() {
  if constexpr (true) {
    foo1();
  } else {
    foo2();
  }
}
