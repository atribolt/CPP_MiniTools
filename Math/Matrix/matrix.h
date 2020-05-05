#ifndef Matrix_H
#define Matrix_H

#include <memory>

namespace Math {

   struct Point {
      size_t x;
      size_t y;
   };

   class zero_divide_exception : std::exception {};

   namespace Tool {
      template<class T, size_t size>
         double MultypleDiagonal(const T (&data)[size], size_t begin, size_t offset, size_t n ) {
            double res = 0
                 , mul = 0;

            size_t row = begin
                 , col = 0;

            for(size_t rStep = 0; rStep < n; ++rStep, row += n) {
               mul = data[row];
               size_t cStep = 1;
               for(col = row + offset; cStep < n; ++cStep, col += offset) {
                  mul *= data[col % size];
               }
               res += mul;
            }
            return res;
         }
   }

   template<class val_t, size_t row, size_t col>
      class Matrix {
      public:
         using value_type = val_t;

      public:
         Matrix() {}

         value_type& operator[](Point elem) { 
            return data[elem.y * col + elem.x];
         }
         const value_type& operator[](Point elem) const { 
            return data[elem.y * col + elem.x];
         }

         value_type* begin() {
            return data;
         }
         value_type* end() {
            return data + row * col;
         }

         template<class _T>
            Matrix& operator=(const Matrix<_T, row, col>& b) {
               std::memcpy(data, b.data, sizeof(value_type) * col * row);
               return *this;
            }

         template<class _T>
            Matrix& operator=(const _T(&dat)[row][col]) {
               for (size_t i = 0; i < row; ++i) {
                  std::memcpy(data + i * col, dat[i], sizeof(value_type) * col);
               }
               return *this;
            }

         template<class _T>
            Matrix& operator=(_T(&&dat)[row][col]) {
               for (size_t i = 0; i < row; ++i) {
                  std::memmove(data + i * col, dat[i], sizeof(value_type) * col);
               }
               return *this;
            }

         template<class _T>
            Matrix&& operator=(Matrix<_T, row, col>&& m) noexcept {
               std::swap(data, m.data);
               return *this;
            }

         template<class _T>
            Matrix operator*(const _T& val) const {
               Matrix res;

               auto res_iter = res  + row*col
                  , dat_iter = data + row*col;

               while( dat_iter != data ) {
                  *(--res_iter) = *(--dat_iter) * val;
               }
               return res;
            }

         template<class _T>
            Matrix operator/(const _T& val)      const {
               if (val == 0) throw zero_divide_exception;

               Matrix res;

               auto res_iter = res  + row * col
                  , dat_iter = data + row * col;

               while (dat_iter != data) {
                  *(--res_iter) = *(--dat_iter) / val;
               }
               return res;
            }

         template<class _T>
            Matrix operator+(const Matrix<_T, row, col>& mat) const {
               Matrix res;

               auto res_iter = res  + row * col
                  , dat_iter = data + row * col
                  , mat_iter = mat  + row * col;

               while (dat_iter != data) {
                  *(--res_iter) = *(--dat_iter) + *(--mat_iter);
               }
               return res;
            }
         template<class _T>
            Matrix operator-(const Matrix<_T, row, col>& mat) const {
               Matrix res;

               auto res_iter = res  + row * col
                  , dat_iter = data + row * col
                  , mat_iter = mat  + row * col;

               while (dat_iter != data) {
                  *(--res_iter) = *(--dat_iter) - *(--mat_iter);
               }
               return res;
            }

         template<class _T, size_t _col>
            Matrix<value_type, _col, col>   operator*(const Matrix<_T, col, _col>& mat) const {
               Matrix<value_type, _col, col> res;

               for (size_t n = 0; n < _col * col; ++n) {
                  size_t r = n / col
                       , c = n % col;

                  res.data[n] = 0;

                  for (size_t i = 0; i < col; ++i) {
                     res.data[n] += (mat.data[c * col + i] * data[i * col + c]);
                  }
               }

               return res;
            }


         Matrix<value_type, row-1, col-1> Remove(Point rc) const {
            Matrix<value_type, row - 1, col - 1> res;

            size_t size = (row-1) * (col-1);

            for (size_t n = 0; n < size; ++n) {
               size_t r = n / (col - 1)
                    , c = n % (col - 1);

               if (r == rc.x || c == rc.y) continue;

               res.data[n] = data[n];
            }

            return res;
         }

         template<class Result = double>
            Result M(Point e) const {
               return Remove(e).Det();
            }

         template<class Result = double>
            Result A(Point e) const {
               return pow(-1, e.x + e.y) * M(e);
            }

         template<class Result = double>
            Result Det() const {
               double res = 0;
               
               res += Tool::MultypleDiagonal(data,     0, row + 1, row);
               res -= Tool::MultypleDiagonal(data, row-1, row - 1, row);

               return res;
            }

         constexpr auto Rows() const { return row; }
         constexpr auto Cols() const { return col; }
      private:
         value_type data[row * col];
      };
}

//
//using namespace std;
//using Elem = std::pair<size_t, size_t>;
//using Data = std::vector<double>;
//
//#define E_DEF(f,s)            \
//        Elem e((f),(s));      \
//        auto& [row, col] = e
//
//#ifdef _DEBUG
//#define Assert(x) if(!x) std::cout << #x << '\n';
//#elif
//#define Assert(x) assert(x);
//#endif
//
//class Matrix {
//public:
//  Matrix ( ) { }
//
//  Matrix(size_t row, size_t col) noexcept;
//  Matrix(const Matrix& m);
//  Matrix(Matrix&& m) noexcept;
//
//  double M(Elem e) const;   // minor
//  double A(Elem e) const;   // algebraic complement
//  double Det() const;       // determinant
//
//  size_t Row()  const noexcept;
//  size_t Col()  const noexcept;
//  size_t Size() const noexcept;
//
//  double& operator[](Elem e);
//  const double& operator[](Elem e) const;
//
//  double& operator[](size_t e);
//  const double& operator[](size_t e) const;
//
//  Matrix& operator=(const Matrix& b);
//  Matrix& operator=(const vector<double>& vec);
//  Matrix&& operator=(Matrix&& m) noexcept;
//
//  Matrix operator*(double val) const;
//  Matrix operator/(double val) const;
//  Matrix operator+(const Matrix& b) const;
//  Matrix operator-(const Matrix& b) const;
//  Matrix operator*(const Matrix& b) const;
//
//  Matrix Cut(Elem cutRowCol) const;
//
//  ~Matrix();
//private:
//  double Det(Elem start) const;
//
//  void Swap(Matrix&& m);
//  void Copy(const Data& data, size_t count);
//  void Alloc(size_t r, size_t c);
//  void Clean();
//
//  size_t r{0}; // row
//  size_t c{0}; // column
//
//  Data data;
//};
//
//ostream& operator<<(ostream& os, const Matrix& m);
//Matrix GetMatrix();

#endif