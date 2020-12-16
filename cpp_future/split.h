#pragma once

#include <xstring>

template<class _Elem, class _Trait = std::char_traits<_Elem>, class _Alloc = std::allocator<_Elem>, class _FwIter>
  _FwIter split(std::basic_string<_Elem, _Trait, _Alloc> const& string, std::basic_string<_Elem, _Trait, _Alloc> delimetr, _FwIter dst) {
    size_t begin = 0;
    for (size_t i = 0; i < (string.size() - delimetr.size()); ++i) {
      size_t buffer = i;
      for (size_t j = 0; j < delimetr.size() && (string[i] == delimetr[j]); ++j, ++i);

      if ((i - buffer) == delimetr.size()) {
        *dst = string.substr(begin, buffer - begin);
        dst++;
        begin = i;
      }
    }

    if (begin < string.size()) {
      *dst = string.substr(begin, (string.size() - begin));
      dst++;
    }

    return dst;
  }

/*
  #include <vector>
  #include <string>
  #include <iterator>

  int main() {
    std::vector<std::string> outRange;
    std::string source = "a|-|b|-|c|-|def|-|j";

    auto endIter = split(source, std::string("|-|"), std::back_inserter(outRange));

    // outRange: [a, b, c, def, j]
    // endIter == outRange.end();
  }
*/