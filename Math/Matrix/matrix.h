#pragma once

#include <vector>

namespace Math {

class Matrix {
public:
  template<class T>
    using data_type_impl = std::vector<T>;

  using value_type = double;
  using data_type = data_type_impl<value_type>;
  using point_type = std::pair<size_t, size_t>;

  Matrix();
  Matrix(const Matrix& mat);
  Matrix(size_t row, size_t col);
  ~Matrix();

  void init(size_t r, size_t c);
  void clear();
  double minor(point_type elem) const;
  double determine() const;
  double algebraic(point_type elem) const;
  Matrix cut(point_type cutRowCol) const;

  size_t rows() const;
  size_t cols() const;
  size_t size() const;

public:
  double& operator[](point_type elem);
  const double& operator[](point_type elem) const;

  double& operator[](size_t index);
  const double& operator[](size_t index) const;

  Matrix& operator=(const data_type& data);
  Matrix& operator=(const Matrix& mat);
  Matrix& operator*=(value_type val);
  Matrix& operator/=(value_type val);
  Matrix& operator+=(const Matrix& b);
  Matrix& operator-=(const Matrix& b);

private:
  void swap(Matrix&& m);

  size_t _rows = { 0 };
  size_t _cols = { 0 };
  data_type _data;
};

Matrix operator*(const Matrix& a, Matrix::value_type val);
Matrix operator/(const Matrix& a, Matrix::value_type val);
Matrix operator+(const Matrix& a, const Matrix& b);
Matrix operator-(const Matrix& a, const Matrix& b);
Matrix operator*(const Matrix& a, const Matrix& b);

}