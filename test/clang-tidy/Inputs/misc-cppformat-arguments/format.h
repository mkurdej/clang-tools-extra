#ifndef FORMAT_H_
#define FORMAT_H_

#define FMT_VARIADIC(ReturnType, Name, ...) \
  template <typename... Args>               \
  void Name(__VA_ARGS__, const Args &... args);

#define FMT_VARIADIC_W(ReturnType, Name, FirstArguments) FMT_VARIADIC(ReturnType, Name, FirstArguments)

//#include <cstdio>
//#include <ostream>
//#include <string>

class Color;
using CStringRef = const char *;
using WCStringRef = const wchar_t *;
namespace std {
class FILE;
class ostream;
class string;
class wstring;
}

namespace fmt {
FMT_VARIADIC(std::string, format, CStringRef)
FMT_VARIADIC_W(std::wstring, format, WCStringRef)
FMT_VARIADIC(void, print, CStringRef)
FMT_VARIADIC(void, print, std::FILE *, CStringRef)
FMT_VARIADIC(void, print, std::ostream &, CStringRef)
FMT_VARIADIC(void, print_colored, Color, CStringRef)
//FMT_VARIADIC(std::string, sprintf, CStringRef)
//FMT_VARIADIC(int, printf, CStringRef)
//FMT_VARIADIC(int, fprintf, std::FILE *, CStringRef)
}

#endif // FORMAT_H_
