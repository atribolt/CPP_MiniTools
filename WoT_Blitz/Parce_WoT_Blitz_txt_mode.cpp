#include "Parce_WoT_Blitz_txt_mode.h"

#include <ctime>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>

using namespace std;
static Point ZERO { 0, 0 };

static tm __fastcall TimeNow() {
   time_t _sec {0};
   tm _now;

   time(&_sec);
   localtime_s(&_now, &_sec);
   return _now;
}

static inline void LogMessage(string message) noexcept {
#ifdef _DEBUG
   tm t = TimeNow();
   // dd.mm|hh:mm:ss >>> message
   cerr << t.tm_mday << "."
        << t.tm_mon + 1  << "|"
        << t.tm_hour << ":" << t.tm_min << ":" << t.tm_sec << " >>> "
        << message << '\n';
#endif
}

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

ostream& operator<<(ostream& os, const DataManager& dm) noexcept {
   os << dm._data;
   return os;
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

PathAtlases FindAtlasImage(Data& _data, const Path& path) {
  DataModify md(_data);
  PathAtlases atlases;
  auto dir = path.parent_path().u8string();

  for(auto& [tex, size] : md._texs) {
      string ptex = tex;
      ptex.resize(ptex.size() - 3);
      ptex += "png";

      atlases.push_back( { dir + "\\" + ptex } );
  }
  return atlases;
}


#pragma region >>> DataManager
template<class T>
bool DataManager::TestIndex(T& _elem, size_t index) noexcept {
   return (index < _elem.size());
}

DataManager::DataManager(const Path& path) noexcept 
   : _work_directory(path)
   , _current_atlas(0)
   , _current_layer(0)
{
   ifstream read_data(_work_directory);
   if(read_data) {
      read_data >> _data;
      _atlas_images = FindAtlasImage(_data, _work_directory);
      
      _error_flag = false;
      read_data.close();
   }
   else {
      LogMessage("Ошибка открытия файла \'" + _work_directory.string() + "\'");
      _error_flag = true;
   }
}

void DataManager::MoveTile(int dx, int dy) noexcept {
   if(!_error_flag) {
      LayerModify _l(_data, _current_layer);

      auto& [x, y] = _l.offset;
      x += dx;
      y += dy;
   } else {
      LogMessage("Объект в состоянии ошибки. MoveTile(int,int) невозможна");
   }
}

void DataManager::Center(int x, int y) noexcept {
   if(!_error_flag) {
      auto& [_x, _y] = LayerModify(_data, _current_layer).offset;
      _x = x;
      _y = y;
   } else {
      LogMessage("Объект в состоянии ошибки. Center(int,int) невозможна");
   }
}

const Point& DataManager::Center() const noexcept {
   return (!_error_flag) ? get<3>(get<1>(_data)[_current_layer]) : ZERO;
}

void DataManager::Position(int x, int y) noexcept {
   if(!_error_flag) {
      auto& [_x, _y] = LayerModify(_data, _current_layer).position;
      _x = x;
      _y = y;
   } else {
      LogMessage("Объект в состоянии ошибки. Операция Position(int,int) невозможна");
   }
}

const Point& DataManager::Position() const noexcept {
   return (!_error_flag) ? get<0>(get<1>(_data)[_current_layer]) : ZERO;
}

void DataManager::Size(int width, int height) noexcept {
   if(!_error_flag) {
      auto& [_w, _h] = LayerModify(_data, _current_atlas).size;
      _w = width;
      _h = height;
   } else {
      LogMessage("Объект в состоянии ошибки. Операция Size(int,int) невозможна");
   }
}

const Point& DataManager::Size() const noexcept {
   return (!_error_flag) ? get<1>(get<1>(_data)[_current_layer]) : ZERO;
}

string DataManager::CurentLayer() const noexcept {
   string res;

   if(!_error_flag) {
      stringstream ss;
      ss << DataModify(_data)._layers.at(_current_layer);
      getline(ss, res);
   } else {
      LogMessage("Объект в состоянии ошибки. Операция CurentLayer() невозможна");
   }
   
   return res;
}

bool DataManager::ChangeTex(size_t index) noexcept {
   if(_error_flag) {
      LogMessage("Объект в состоянии ошибки. Операция ChangeTex(size_t) невозможна");
      return false;
   }
   else {
      if(TestIndex(DataModify(_data)._texs, index)) {
         _current_atlas = index;
         return true;
      } else {
         LogMessage("Выбор несуществующего атласа");
         return false;
      }
   }
}

bool DataManager::ChangeLayer(size_t index) noexcept {
   if(_error_flag) {
      LogMessage("Объект в состоянии ошибки. Операция ChangeLayer(size_t) невозможна");
      return false;
   }
   else {
      if(TestIndex(DataModify(_data)._layers, index)) {
         _current_layer = index;
         return true;
      } else {
         LogMessage("Выбор несуществующего слоя");
         return false;
      }
   }
}

Path DataManager::CurrentAtlas() const noexcept {
   return (!_error_flag) ? _atlas_images[_current_atlas] : "";
}

Names DataManager::AllLayers() const noexcept {
   Names res;
   if(!_error_flag) {
      string tmp;
      stringstream ss;

      ss << DataModify(_data)._layers;
      while(getline(ss, tmp)) {
         res.push_back(tmp);
      }
   } else {
      LogMessage("Объект в состоянии ошибки. Операция AllLayers() невозможна");
   }
   return res;
}

#pragma endregion