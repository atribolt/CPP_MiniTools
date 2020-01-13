#include "Parce_WoT_Blitz_txt_mode.h"

#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>

using namespace std;

#pragma region >>> Обработка ввода вывода
namespace std {
   istream& operator>>(istream& is, Point& p) {
      is >> p.first >> p.second;
      return is;
   }
   istream& operator>>(istream& is, _layer& l) {
      auto& [pos, size, offset, img, index] = l;
      is >> pos >> size >> offset >> img >> index;
      return is;
   }

   ostream& operator<<(ostream& os, const Point& p) noexcept {
      os << p.first << ' ' << p.second;
      return os;
   }
   ostream& operator<<(ostream& os, const _tex& t) noexcept {
      const auto& [name, size] = t;
      os << "File: " << name << "; Size atlas: " << size << ";";
      return os;
   }
   ostream& operator<<(ostream& os, const _layer& l) noexcept {
      const auto& [pos, size, offset, img, index] = l;
      os << pos << ' ' << size << ' ' << offset << ' ' << img << ' ' << index;
      return os;
   }
}

ostream& operator<<(ostream& os, const Texs& texs) noexcept {
   Names names;
   Points sizes;

   Unpack_Texs(begin(texs), end(texs), back_inserter(names), back_inserter(sizes));

   copy(begin(names), end(names), ostream_iterator<string>(os, "\n"));
   copy(begin(sizes), end(sizes), ostream_iterator<Point>(os, "\n"));

   return os;
}
ostream& operator<<(ostream& os, const Layers& ls) noexcept {
   copy(begin(ls), end(ls), ostream_iterator<_layer>(os, "\n"));
   //for(auto& l: ls) os << l << '\n';
   return os;
}
ostream& operator<<(ostream& os, const Data& data) noexcept {
   const auto& [texs, layers] = data;

   os << texs.size() << '\n'
      << texs 
      << layers.size() << '\n'
      << layers;

   return os;
}

istream& operator>>(istream& is, Texs& t) {
   size_t count {0}; // кол-во файлов
   is >> count;

   Names  _n{count, ""};    // имена файлов
   Points _s{count, {0,0}}; // размеры

   copy_n(istream_iterator<string>{is}, count, begin(_n));
   copy_n(istream_iterator<Point>{is}, count, begin(_s));

   Pack_Texs(_n, _s, back_inserter(t));
   return is;
}
istream& operator>>(istream& is, Layers& l){
   size_t count {0};
   is >> count;
   l.resize(count);
   copy(istream_iterator<_layer>{is}, {}, begin(l));
   return is;
}
istream& operator>>(istream& is, Data& d) {
   auto& [texs, layers] = d;
   is >> texs;
   is >> layers;
   return is;
}
#pragma endregion

template<class _out_name, class _out_size>
void Unpack_Texs ( 
   Texs::const_iterator src_beg, 
   Texs::const_iterator src_end, 
   _out_name out_name,           
   _out_size out_size)           
{
   auto unpack = [&out_name, &out_size](auto& t) { 
      auto& [name, size] = t;
      *out_name++ = name;
      *out_size++ = size;
   };

   for_each(src_beg, src_end, unpack);
}

template<class _it_where>
void Pack_Texs (const Names& _names, const Points& _sizes, _it_where where_to ) {
   auto _n_beg = begin(_names);
   auto _s_beg = begin(_sizes);

   for(; _n_beg != end(_names) && _s_beg != end(_sizes); ++_n_beg, ++_s_beg, ++where_to) {
      *where_to = { *_n_beg, *_s_beg };
   }
}

TexModify::TexModify (Data& _data, size_t index) :
   file   (get<0>(get<0>(_data)[index])),
   width  ((get<1>(get<0>(_data)[index]).first)),
   height ((get<1>(get<0>(_data)[index]).second))
{}

LayerModify::LayerModify(Data& _data, size_t index) :
   position (get<0>(get<1>(_data)[index])),
   size     (get<1>(get<1>(_data)[index])),
   offset   (get<2>(get<1>(_data)[index])),
   atlas    (get<3>(get<1>(_data)[index])),
   name     (get<4>(get<1>(_data)[index]))
{}

DataModify::DataModify(Data& data) :
   _texs(get<0>(data)),
   _layers(get<1>(data))
{}