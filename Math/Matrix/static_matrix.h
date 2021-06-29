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


   template<class T, size_t step = 1>
      class mat_iterator : public std::random_access_iterator_tag {
      public:
         using value_type = T;

         mat_iterator() {}
         mat_iterator(T* data) noexcept : data(data) {}

         value_type& operator*() { return *data; }
         auto& operator++() {
            data += step;
            return *this;
         }

         const value_type& operator[](size_t index) const {
            return *(data + (step * index));
         }
         value_type& operator[](size_t index) {
            return *(data + (step * index));
         }

         auto& operator--() {
            data -= step;
            return *this;
         }

         auto operator+(int i) {
            mat_iterator res = *this;
            res.data += (i * step);
            return res;
         }
         auto operator-(int i) {
            mat_iterator res = *this;
            res.data -= (i * step);
            return res;
         }
         bool operator==(const mat_iterator& b) const { return data == b.data; }
         bool operator!=(const mat_iterator& b) const { return data != b.data; }
      private:
         value_type* data { nullptr };
      };

   template<class T, size_t step>
      class Vector {
      public:
         using value_type = T;

         Vector(size_t size) {
            Resize(size);
         }
         Vector(mat_iterator<T, step> begn, size_t size) {
            beg = begn;
            Resize(size);
         }

         auto begin() const { return beg; }
         auto end()   const { return beg + Size(); }

         auto& operator[](size_t index) { return beg[index]; }

         auto Size() const { return size; }
         void Resize(size_t now) { size = now; }
      private:
         size_t                         size;
         mat_iterator<value_type, step> beg;
      };
   template<class val_t, size_t row, size_t col>
      class Matrix {
      public:
         using value_type = val_t;

      public:
         using row_iterator = mat_iterator<value_type,   1>;
         using col_iterator = mat_iterator<value_type, col>;
         using row_iter_c   = const row_iterator;
         using col_iter_c   = const col_iterator;

         Matrix() {}

         value_type& operator[](Point elem) { 
            return data[elem.y * col + elem.x];
         }
         const value_type& operator[](Point elem) const { 
            return data[elem.y * col + elem.x];
         }

         auto begin() {
            return mat_iterator( data );
         }
         auto end() {
            return mat_iterator( data + (col * row) );
         }

         auto beg_row(size_t index) {
            return row_iterator(data + index * col);
         }
         auto beg_col(size_t index) {
            return col_iterator(data + index);
         }
         auto end_row(size_t index) {
            return row_iterator(data + index * col + col);
         }
         auto end_col(size_t index) {
            return col_iterator(data + index + col * row);
         }

         auto Row(size_t index) {
            return Vector<value_type, 1>(beg_row(index), col);
         }
         auto Column(size_t index) {
            return Vector<value_type, col>(beg_col(index), row);
         }

         template<class _T>
            Matrix& operator=(const Matrix<_T, row, col>& b) {
               std::memcpy(data, b.data, sizeof(value_type) * col * row);
               return *this;
            }

         template<class _T>
            Matrix& operator=(const _T(&dat)[row][col]) {
               for (size_t i = 0; i < row; ++i) {
                  data[i] = dat[i / col][i % col];
               }
               return *this;
            }

         template<class _T>
            Matrix& operator=(_T(&&dat)[row][col]) {
               for (size_t i = 0; i < row; ++i) {
                  std::swap(data[i], dat[i / col][i % col]);
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

         auto operator[](size_t column) { return col_iterator(column); }
         auto operator[](size_t column) const { return col_iter_c(column); }

       	value_type Max() const {
       	   value_type res = 0;
       	 	
       	   for ( auto& el : *this ) {
       	 	   if ( res < el ) { res = el; }
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

      enum class TypeIter {
         Row, Col
      };

      

      template<class At, class Bt, size_t stepA, size_t stepB>
         At operator*(Vector<At, stepA> a, Vector<Bt, stepB> b) {
            At res = 0;

            for (size_t i = 0; i < a.Size(); ++i) {
               res += a[i] * static_cast<At>(b[i]);
            }

            return res;
         }
      template<class At, size_t stepA, class Iter>
         At operator*(Vector<At, stepA> a, Iter it) {
            At res = 0;

            for (size_t i = 0; i < a.Size(); ++i, ++it) {
               res += a[i] * static_cast<At>(*it);
            }

            return res;
         }
      template<class At, size_t stepA, class Iter>
         At operator*(Iter it, Vector<At, stepA> a) {
            At res = 0;

            for (size_t i = 0; i < a.Size(); ++i, ++it) {
               res += a[i] * static_cast<At>(*it);
            }

            return res;
         }

}
