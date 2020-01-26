#ifndef WOT_MODIFY_DATA_H
#define WOT_MODIFY_DATA_H

#include <tuple>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

using Point       = std::pair<int, int>;
using _tex        = std::tuple<std::string, Point>;
using _layer      = std::tuple<Point, Point, Point, int, std::string>;
using Points      = std::vector<Point>;
using Names       = std::vector<std::string>;
using Texs        = std::vector<_tex>;
using Layers      = std::vector<_layer>;
using Data        = std::tuple<Texs, Layers>;
using Path        = std::filesystem::path;
using PathAtlases = std::vector<Path>;

class DataManager;

namespace std {
   /* Необходимы в std для поддержки алгоритмами stl */

   istream& operator>>(istream&, Point&);
   istream& operator>>(istream&, _layer&);

   ostream& operator<<(ostream&, const _tex&)   noexcept;
   ostream& operator<<(ostream&, const Point&)  noexcept;
   ostream& operator<<(ostream&, const _layer&) noexcept;
}
std::ostream& operator<<(std::ostream&, const DataManager&) noexcept;
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

class DataManager {
   friend  std::ostream& operator<<(std::ostream&, const DataManager&) noexcept;

   Path        _work_directory;
   PathAtlases _atlas_images;

   size_t 
      _current_layer,
      _current_atlas;

   mutable Data _data;
   bool _error_flag;

   template<class T>
   bool TestIndex(T& _elem, size_t index) noexcept;
public:
   DataManager(const Path& path) noexcept;

#pragma region >>> Управление слоем
   /** Перемещение центра */
   void MoveTile(int dx, int dy) noexcept;

   /** Установка нового центра для тайла */
   void Center(int x, int y) noexcept;
   /** Текущий центр тайла */
   const Point& Center() const noexcept;

   /** Установка новой позиции тайла на атласе */
   void Position(int x, int y) noexcept;
   /** Текущая позиция тайла на атласе */
   const Point& Position() const noexcept;

   /** Установка размера тайла */
   void Size(int width, int height) noexcept;
   /** Текущий размер тайла */
   const Point& Size() const noexcept;

   /** Текущий слой текстом */
   std::string CurentLayer() const noexcept;
#pragma endregion


   /** Выбор атласа */
   bool ChangeTex(size_t index) noexcept;
   /** Выбор слоя */
   bool ChangeLayer(size_t index) noexcept;

   Path CurrentAtlas() const noexcept;

   Names AllLayers() const noexcept;
};

#endif // WOT_MODIFY_DATA_H