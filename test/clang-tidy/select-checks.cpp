// RUN: clang-tidy %s -checks='-*,llvm-namespace-*' -- 2>&1 | FileCheck -implicit-check-not='{{warning:|error:}}' %s
// RUN: clang-tidy %s -checks='-*,an-unknown-check' -- 2>&1 | FileCheck -implicit-check-not='{{warning:|error:}}' -check-prefix=CHECK2 %s

// CHECK2: Error: no checks enabled.

namespace i {
}
// CHECK: :[[@LINE-2]]:11: warning: namespace not terminated with a closing comment [llvm-namespace-comment]

// Expect no warnings from the google-explicit-constructor check:
class A { A(int i); };
