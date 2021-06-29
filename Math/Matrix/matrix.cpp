#include <cassert>

#include "matrix.h"

namespace Math {

Matrix::Matrix() {
}

Matrix::Matrix(size_t row, size_t col) {
  init(row, col); 
}

Matrix::Matrix(const Matrix& mat) {
  *this = mat;
}

Matrix::~Matrix() {
}

double Matrix::minor(point_type e) const {
  Matrix m = this->cut(e);
  return m.determine();
}

double Matrix::algebraic(point_type e) const {
  auto& [row, col] = e;
  return pow(-1, row + col) * (this->minor(e));
}

static double multypleDiagonal(const typename Matrix::data_type& data, 
                        size_t begin,
                        size_t offset,
                        size_t n)
{
  double 
    res = 0,
    mul = 0;
  size_t row = begin,
          col = 0;

  for(size_t rStep = 0; rStep < n; ++rStep, row += n) {
    mul = data[row];
    size_t cStep = 1;
    for(col = row + offset; cStep < n; ++cStep, col += offset) {
      mul *= data[col % data.size()];
    }
    res += mul;
  }
  return res;
}

double Matrix::determine() const {
  if (rows() != cols()) {
    return 0.0;
  }

  if (rows() == 1) {
    return _data[0];
  }
  else if(rows() == 2) {
    return _data[0] * _data[3] - _data[1] * _data[2];
  }

  double res  = multypleDiagonal(_data,          0, rows() + 1, rows());
          res -= multypleDiagonal(_data, rows() - 1, rows() - 1, rows());

  return res;
}

Matrix& Matrix::operator=(const data_type& data) {
  assert(data.size() == _cols * _rows);
  _data = data;
  return *this;
}

Matrix& Matrix::operator=(const Matrix& mat) {
  init(mat.rows(), mat.cols());
  _data = mat._data;
  return *this;
}

double& Matrix::operator[](point_type e) {
  const auto& [row, col] = e;
  assert(row < rows() && col < cols());
  return _data[row * cols() + col];
}

const double& Matrix::operator[](point_type e) const {
  const auto& [row, col] = e;
  assert(row < rows() && col < cols());
  return _data[row * cols() + col];
}

double& Matrix::operator[](size_t e) {
  assert(e < size());
  return _data[e];
}

const double& Matrix::operator[](size_t e) const {
  assert(e < size());
  return _data[e];
}

size_t Matrix::rows() const { 
  return _rows; 
}

size_t Matrix::cols() const { 
  return _cols; 
}

size_t Matrix::size() const { 
  return rows() * cols(); 
}

Matrix& Matrix::operator+=(const Matrix& b) {
  assert(rows() == b.rows() && cols() == b.cols());

  for (size_t i = 0; i < size(); ++i) {
    _data[i] += b._data[i];
  }

  return *this;
}

Matrix& Matrix::operator-=(const Matrix& b) {
  assert(rows() == b.rows() && cols() == b.cols());
  
  for (size_t i = 0; i < size(); ++i) {
    _data[i] -= b._data[i];
  }

  return *this;
}
  
Matrix& Matrix::operator*=(value_type val) {    
  for (size_t i = 0; i < size(); ++i) {
    _data[i] *= val;
  }
  return *this;
}

Matrix& Matrix::operator/=(value_type val) {    
  assert(val != 0);
  for (size_t i = 0; i < size(); ++i) {
    _data[i] /= val;
  }
  return *this;
}

Matrix Matrix::cut(point_type cutRowCol) const {
  auto& [skipRow, skipCol] = cutRowCol;

  assert(skipRow < rows() && skipCol < cols());

  Matrix m(rows() - 1, cols() - 1);

  for(size_t row = 0, dr = 0 ; row < rows(); ++row, ++dr) {
    if (row == skipRow) {
      continue;
    }

    for(size_t col = 0, dc = 0; col < cols(); ++col, ++dc) {
      if (col == skipCol) {
        continue;
      }
      m[{dr, dc}] = (*this)[{row, col}];
    }
  }
  return m;
}

void Matrix::swap(Matrix&& m) {
  _data.swap(m._data);
  
  std::swap(_rows, m._rows);
  std::swap(_cols, m._cols);
}

void Matrix::init(size_t r, size_t c) {
  _rows = r;
  _cols = c;

  _data.resize(size());
}

void Matrix::clear() {
  _data.clear();
  _rows = _cols = 0;
}

Matrix operator*(const Matrix& a, const Matrix& b) {
  assert(a.cols() == b.rows());
  
  Matrix result(a.rows(), b.cols());

  size_t row = 0, 
          col = 0;

  for(row = 0; row < result.rows(); ++row) {
    for(col = 0; col < result.cols(); ++col) {
      result[{row, col}] = 0;
      for(size_t index = 0; index < a.cols(); ++index) {
        result[{row, col}] += a[{row, index}] * b[{index, col}];
      }
    }
  }
  return result;
}

Matrix operator*(const Matrix& a, Matrix::value_type val) {
  Matrix res(a);
  res *= val;
  return res;
}


Matrix operator/(const Matrix& a, Matrix::value_type val) {
  Matrix res(a);
  res /= val;
  return res;
}

Matrix operator+(const Matrix& a, const Matrix& b) {
  Matrix res(a);
  res += b;
  return res;
}

Matrix operator-(const Matrix& a, const Matrix& b) {
  Matrix res(a);
  res -= b;
  return res;
}
}