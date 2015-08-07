// RUN: $(dirname %s)/check_clang_tidy.sh %s misc-use-using %t
// REQUIRES: shell

class Class {};

using my_int_u = int;
using function_u = void (*)(int);
using method_u = void (Class::*)(int);

// CHECK-NOT: warning:
typedef int my_int_t;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: use 'using' instead of 'typedef' [misc-use-using]
// CHECK-FIXES: {{^}}using my_int_t = int;{{$}}

typedef int* my_int_p1_t;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: use 'using' instead of 'typedef' [misc-use-using]
// CHECK-FIXES: {{^}}using my_int_p1_t = int*;{{$}}

typedef int *my_int_p2_t;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: use 'using' instead of 'typedef' [misc-use-using]
// CHECK-FIXES: {{^}}using my_int_p2_t = int *;{{$}}

struct Struct {
  typedef int my_int_member_t;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: use 'using' instead of 'typedef' [misc-use-using]
  // CHECK-FIXES: {{^}}  using my_int_member_t = int;{{$}}
};

typedef void (*function_t)(int);
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: use 'using' instead of 'typedef' [misc-use-using]
// CHECK-FIXES: {{^}}using function_t = void (*)(int);{{$}}

typedef void (Struct::*method_t)(int);
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: use 'using' instead of 'typedef' [misc-use-using]
// CHECK-FIXES: {{^}}using method_t = void (Struct::*)(int);{{$}}

typedef int m_int, *m_int_p, &m_int_r, m_int_arr[10], (&m_int_fun)(int, int);
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: use 'using' instead of 'typedef' [misc-use-using]
