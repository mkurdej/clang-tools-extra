// RUN: %check_clang_tidy %s boost-use-make-unique %t

namespace boost {

template <class T>
class unique_ptr {};

template <class T, class... Args>
unique_ptr<T> make_unique(Args &&... args) { return {}; }

} // namespace boost

namespace std {

template <class T>
class unique_ptr {};

template <class T, class... Args>
unique_ptr<T> make_unique(Args &&... args) {
  return {};
}

} // namespace std

struct MyStruct {
  MyStruct() {}
  MyStruct(int) {}
  MyStruct(double, float) {}
  MyStruct(double, MyStruct, float) {}
};

void fPositive() {
  boost::make_unique<MyStruct>();
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: use std::make_unique instead of boost::make_unique [boost-use-make-unique]
  // CHECK-FIXES: std::make_unique<MyStruct>();{{$}}
}

void fPositiveUsing() {
  using boost::make_unique;
  make_unique<MyStruct>(42);
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: use std::make_unique instead of boost::make_unique [boost-use-make-unique]
}

void fNegative() {
  std::make_unique<MyStruct>();
}
