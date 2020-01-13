#ifndef WOT_MODIFY_DATA_H
#define WOT_MODIFY_DATA_H

#include <tuple>
#include <string>
#include <vector>
#include <iostream>

using Point  = std::pair<int, int>;
using _tex   = std::tuple<std::string, Point>;
using _layer = std::tuple<Point, Point, Point, int, std::string>;
using Points = std::vector<Point>;
using Names  = std::vector<std::string>;
using Texs   = std::vector<_tex>;
using Layers = std::vector<_layer>;
using Data   = std::tuple<Texs, Layers>;

namespace std {
   /* Необходимы в std для поддержки алгоритмами stl */

   istream& operator>>(istream&, Point&);
   istream& operator>>(istream&, _layer&);

   ostream& operator<<(ostream&, const _tex&)   noexcept;
   ostream& operator<<(ostream&, const Point&)  noexcept;
   ostream& operator<<(ostream&, const _layer&) noexcept;
}
std::ostream& operator<<(std::ostream&, const Data&) noexcept;
std::ostream& operator<<(std::ostream&, const Texs&) noexcept;
std::ostream& operator<<(std::ostream&, const Layers&) noexcept;

std::istream& operator>>(std::istream&, Data&);
std::istream& operator>>(std::istream&, Texs&);
std::istream& operator>>(std::istream&, Layers&);

/* распаковывает пакет с парами имен и размеров атласов в пакет имен и пакет размеров */
template<class _out_name, class _out_size>
void Unpack_Texs (
   Texs::const_iterator src_beg,
   Texs::const_iterator src_end,
   _out_name dst_names,         
   _out_size dst_sizes          
);

/* Упаковывает размеры и имена атласов из пакета имен и пакета размеров */
template<class _it_where>
void Pack_Texs(
   const Names&  src_names,
   const Points& src_sizes,
   _it_where     dst_texs  
);

/* Редактирование отдельного атласа */
struct TexModify {
   std::string& file;
   int& width;
   int& height;

   TexModify(Data&, size_t);
};

/* Редактирование отдельного слоя */
struct LayerModify {
   Point&  position;
   Point&  size;
   Point&  offset;
   int&    atlas;
   std::string& name;

   LayerModify(Data&, size_t);
};

/* Редактирование списка атласов и слоев */
struct DataModify {
   Texs& _texs;
   Layers& _layers;

   DataModify(Data&);
};
#endif