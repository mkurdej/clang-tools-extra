// RUN: %check_clang_tidy %s bugprone-suspicious-semicolon %t -- -- -std=c++17

void fail_if_constexpr() {
  if constexpr (true)
    ;
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: potentially unintended semicolon
}

void correct_if_constexpr() {
  if constexpr (true) {
  }
}
