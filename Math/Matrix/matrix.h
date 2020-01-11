#ifndef Matrix_H
#define Matrix_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <memory.h>

using namespace std;
using Elem = std::pair<size_t, size_t>;
using Data = std::vector<double>;

#define E_DEF(f,s)            \
        Elem e((f),(s));      \
        auto& [row, col] = e

#ifdef _DEBUG
#define Assert(x) if(!x) std::cout << #x << '\n';
#elif
#define Assert(x) assert(x);
#endif

class Matrix {
public:
  Matrix ( ) { }

  Matrix(size_t row, size_t col) noexcept;
  Matrix(const Matrix& m);
  Matrix(Matrix&& m) noexcept;

  double M(Elem e) const;   // minor
  double A(Elem e) const;   // algebraic complement
  double Det() const;       // determinant

  size_t Row()  const noexcept;
  size_t Col()  const noexcept;
  size_t Size() const noexcept;

  double& operator[](Elem e);
  const double& operator[](Elem e) const;

  double& operator[](size_t e);
  const double& operator[](size_t e) const;

  Matrix& operator=(const Matrix& b);
  Matrix& operator=(const vector<double>& vec);
  Matrix&& operator=(Matrix&& m) noexcept;

  Matrix operator*(double val) const;
  Matrix operator/(double val) const;
  Matrix operator+(const Matrix& b) const;
  Matrix operator-(const Matrix& b) const;
  Matrix operator*(const Matrix& b) const;

  Matrix Cut(Elem cutRowCol) const;

  ~Matrix();
private:
  double Det(Elem start) const;

  void Swap(Matrix&& m);
  void Copy(const Data& data, size_t count);
  void Alloc(size_t r, size_t c);
  void Clean();

  size_t r{0}; // row
  size_t c{0}; // column

  Data data;
};

ostream& operator<<(ostream& os, const Matrix& m);
Matrix GetMatrix();

#endif