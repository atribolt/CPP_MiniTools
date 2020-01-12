/*
    Данный файл содержит код для работы с текстовыми фалами,
    используемыми в World of Tanks Blitz.

      Структура такого файла:
      <кол-во атласов> N\r\n
      <имена атласов> Name1.tex\r\n ...\r\n NameN.tex\r\n
      <размеры атласов> 100 100\r\n... ...\r\n
      <кол-во слоев> Nl\r\n
      <описание слоев>0 0 0 0 0 0 0 0
      
      Пример:
      1
      Name.tex
      10 10
      1
      0 0 10 10 0 0 0 0
      
      Имена атласов 
        - это имя изображения до первой точки. Формат описывается как tex.
          По одному имени на строку. Имен ровно столько, сколько указано в первой строчке.
      Размер атласов 
        - это два числа через пробел на одно строке.
          строк столько же, стколько имен атласов
      Слой
        - это описание спрайта на общем атласе с щепоткой параметров для отображения.
          Один слой на строку. (однако, если дадите программе буфер, где весь файл в одну строку,
          и параметры разделены пробелом, то буфер также корректно будет прочитан в Data)
          Параметры для слоя
            - первая пара чисел через пробел - позиция спрайта на общем атласе
            - вторая пара через пробел - размер спрайта
            - третья пара - центр спрайта
            - число - номер атласа, на котором расположен спрайт (не более чем подключено атласов)
            - последний параметр может быть числом, а может строкой - номер слоя. 
              когда описан строкой - "frame0" - 0 означает первый слой.
              
    Для простейшего применения данного кода необходим файл со структурой выше.
    Можно открыть поток чтения файла и прочитать сразу в Data.
    Чтобы редактировать полученные данные, необходимо использовать соответствующие структуры.
    Вывод обратно в файл также прост, как и чтение.
*/

#include <cmath>
#include <ctime>
#include <tuple>
#include <string>
#include <vector>
#include <chrono>
#include <cassert>
#include <fstream>
#include <sstream>
#include <variant>
#include <iterator>
#include <queue>
#include <iostream>
#include <optional>
#include <algorithm>
#include <functional>

using namespace std;
//using Dir = std::filesystem::path;

using Point  = pair<int, int>;                          // для описания размеров и позиций
using _tex   = tuple<string, Point>;                    // описание атласа (имя и размер)
using _layer = tuple<Point, Point, Point, int, string>; // описание слоя
using Points = vector<Point>;                           // набор размеров или позиций
using Names  = vector<string>;                          // набор имен атласов (необходим для вывода списка атласов)
using Texs   = vector<_tex>;                            // набор атласов
using Layers = vector<_layer>;                          // набор слоев
using Data   = tuple<Texs, Layers>;                     // описание всего файла

#pragma region >>> Вспомогательные функции
/* 
   распаковывает пакет с парами имен и размеров атласов в пакет имен и пакет размеров
   >> src_beg  ---> начало пакета запакованных структур
   >> src_end  ---> конец пакета запакованных структур
   >> out_name ---> место, куда будут отправляться имена
   >> out_size ---> место, куда будут отправляться размеры
*/
template<class _out_name, class _out_size>
void Unpack_Texs ( 
   Texs::const_iterator src_beg, // начало списка атласов
   Texs::const_iterator src_end, // конец списка атласов
   _out_name out_name,           // место для вставки имен
   _out_size out_size)           // место для вставки размеров
{
   auto unpack = [&out_name, &out_size](auto& t) { 
      auto& [name, size] = t;
      *out_name++ = name;
      *out_size++ = size;
   };
   
   for_each(src_beg, src_end, unpack);
}

/* 
   Упаковывает размеры и имена атласов из пакета имен и пакета размеров
   Необходимо учитывать, что упаковка производится в уже созданные элементы.
   Если пункт назначения - пустой список, то возникнет исключение
   >> _names   ---> пакет имен
   >> _sizes   ---> пакет размеров
   >> where_to ---> пункт назначения запакованных структур
*/
template<class _it_where>
void Pack_Texs (
   const Names& _names,
   const Points& _sizes,
   _it_where where_to )
{
   auto _n_beg = begin(_names);
   auto _s_beg = begin(_sizes);

   for(; _n_beg != end(_names) && _s_beg != end(_sizes); ++_n_beg, ++_s_beg, ++where_to) {
      *where_to = { *_n_beg, *_s_beg };
   }
}
#pragma endregion

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
ostream& operator<<(ostream& os, const Data& data) {
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

#pragma region >>> Изменение свойств объектов
/*
  Редактирование описания одного атласа.
*/
struct TexModify {
   string& file; // имя атласа
   int& width;   // ширина
   int& height;  // высота

   /*
      _data >>> редактируемый объект
      index >>> номер редактируемого атласа
   */
   TexModify (Data& _data, size_t index) :
      file   (get<0>(get<0>(_data)[index])),
      width  ((get<1>(get<0>(_data)[index]).first)),
      height ((get<1>(get<0>(_data)[index]).second))
   {}
};
/*
  Изменение параметров выбранного слоя
*/
struct LayerModify {
   Point&  position; // позиция спрайта
   Point&  size;     // размер спрайта
   Point&  offset;   // центр
   int&    atlas;    // номер атласа
   string& name;     // описание

   /*
      _data >>> редактируемый объект
      index >>> номер изменяемого слоя
   */
   LayerModify(Data& _data, size_t index) :
      position (get<0>(get<1>(_data)[index])),
      size     (get<1>(get<1>(_data)[index])),
      offset   (get<2>(get<1>(_data)[index])),
      atlas    (get<3>(get<1>(_data)[index])),
      name     (get<4>(get<1>(_data)[index]))
   {}
};
/*
  Редактирование списка слоев и атласов
*/
struct DataModify {
   Texs& _texs;
   Layers& _layers;

   DataModify(Data& data) :
      _texs(get<0>(data)),
      _layers(get<1>(data))
   {}
};
#pragma endregion

int main() {
   // Пример использования
   // создание, редактирование и вывод
   
   // набор атласов с размерами
   Texs _texs = {
      { "Tex_1.tex", {10, 50} }, // у этого изменим имя файла на NewFile.tex
      { "Tex_2.tex", {15, 55} },
      { "Tex_3.tex", {20, 60} },
      { "Tex_4.tex", {25, 65} }  // этот удалим
   };
   
   // набор списка слоев
   Layers ls = {
      { {0, 0}, {10, 10}, {0, 0}, 0, "frame0" },
      { {0, 1}, {10, 10}, {0, 0}, 0, "frame1" }, // этот будем менять frame1 -> newName
      { {0, 2}, {10, 10}, {0, 0}, 0, "frame2" },
   };
   
   Data data { _texs, ls };
   
   /* Редактирование */
   {
      LayerModify l_1 { data, 1 };
      l_1.name = "newName";

      TexModify t_0 { data, 0 };
      t_0.file = "NewFile.tex";

      DataModify dm { data };
      dm._texs.pop_back();
   }

   // вывод
   std::cout << data;

   return 0;
}