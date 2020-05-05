//#include "matrix.h"
//
//ostream& operator<<(ostream& os, const Matrix& m) {
//  Elem e(0, 0);
//  for(e.first = 0; e.first < m.Row(); ++e.first) {
//    for(e.second = 0; e.second < m.Col(); ++e.second)
//      os << m[e] << '\t';
//    os << '\n';
//  }
//  return os;
//}
//Matrix GetMatrix () {
//  size_t R, C;
//  
//  cout << "Размер матрицы (строк столбцов) через пробел: "; cin >> R >> C;
//  cout << "Введите матрицу построчно:\n";
//  
//  Matrix m(R, C);
//  string line, buf;
//  stringstream ss;
//  
//  for(size_t e = 0; e < m.Size() && cin >> line; ++e) {
//    m[e] = atof(line.data());
//  }
//  cin.clear();
//  return m;
//}
//  
//Matrix::Matrix(size_t row, size_t col) noexcept {
//  this->Alloc(row, col); 
//}
//Matrix::Matrix(const Matrix& m) {
//  this->Alloc(m.Row(), m.Col());
//  this->Copy(m.data, m.Size());
//}
//Matrix::Matrix(Matrix&& m) noexcept {
//  (*this) = std::move(m);
//}
//
//double Matrix::M(Elem e) const {
//  Matrix m = this->Cut(e);
//  return m.Det();
//}
//double Matrix::A(Elem e) const {
//  auto& [row, col] = e;
//  return pow(-1, row + col) * (this->M(e));
//}
//inline double MultypleDiagonal(
//  const Data& data, 
//  size_t begin,
//  size_t offset,
//  size_t n
//  ) 
//{
//  double 
//    res = 0,
//    mul = 0;
//  size_t 
//    row = begin, // для высчитывания эелемента
//    col = 0;
//    //fO = n + 1, // смещение главной диагонали
//    //sO = n - 1; // смещение побочной
//                  // расчет главной диагонали
//  for(size_t rStep = 0; rStep < n; ++rStep, row += n) {
//    mul = data[row];
//    size_t cStep = 1;
//    for(col = row + offset; cStep < n; ++cStep, col += offset) {
//      mul *= data[col % data.size()];
//    }
//    res += mul;
//  }
//  return res;
//}
//double Matrix::Det() const {
//  Assert(Row() == Col());
//  if(Row() == 1) return data[0];
//  else if(Row() == 2) {
//    return data[0] * data[3] - data[1] * data[2];
//  }
//  // master diagonal
//  double res = MultypleDiagonal(data, 0, Row()+1, Row());
//  // second
//  res -= MultypleDiagonal(data, Row()-1, Row()-1, Row());
//
//  //double 
//  //  res = 0, // конечный результат
//  //  mul = 0; // хранит произведение диагоналей
//  //size_t 
//  //  row = 0, // для высчитывания эелемента
//  //  col = 0,
//  //  fO = Col()+1, // смещение главной диагонали
//  //  sO = Col()-1; // смещение побочной
//  //// расчет главной диагонали
//  //for(size_t rStep = 0; rStep < Row(); ++rStep, row += Col()) {
//  //  mul = data[row];
//  //  size_t cStep = 1;
//  //  for(col = row + fO; cStep < Row(); ++cStep, col += fO) {
//  //    mul *= data[col % Size()];
//  //  }
//  //  res += mul;
//  //}
//  //// расчет побочной диагонали
//  //row = sO;
//  //for(size_t rStep = 0; rStep < Row(); ++rStep, row += Col()) {
//  //  mul = data[row];
//  //  size_t cStep = 1;
//  //  for(col = row + sO; cStep < Row(); ++cStep, col += sO) {
//  //    mul *= data[col % Size()];
//  //  }
//  //  res -= mul;
//  //}
//  return res;
//}
//double Matrix::Det(Elem start) const {
//  bool Det_NoImplement = false;
//  Assert(Det_NoImplement);
//  return 0;
//}
//
//Matrix&& Matrix::operator=(Matrix&& m) noexcept {
//  Swap(std::move(m));
//  return move(*this);
//}
//
//Matrix& Matrix::operator=(const Matrix& b) {
//  this->Clean();
//  this->Alloc(b.Row(), b.Col());
//  this->Copy(b.data, b.Size());
//  return *this;
//}
//
//Matrix& Matrix::operator=(const vector<double>& vec) {
//  Assert(vec.size() == this->Size());
//  //memcpy(this->data, vec.data(), vec.size() * sizeof(vec[0]));
//  data = vec;
//  return *this;
//}
//double& Matrix::operator[](Elem e) {
//  const auto& [row, col] = e;
//  assert(row < Row() && col < Col());
//
//  return this->data[row * Col() + col];
//}
//const double& Matrix::operator[](Elem e) const {
//  const auto& [row, col] = e;
//  assert(row < this->Row() && col < this->Col());
//
//  return this->data[row * this->Col() + col];
//}
//double& Matrix::operator[](size_t e) {
//  assert(e < this->Size());
//  return this->data[e];
//}
//const double& Matrix::operator[](size_t e) const {
//  assert(e < this->Size());
//  return this->data[e];
//}
//
//
//size_t Matrix::Row() const noexcept { return this->r; }
//size_t Matrix::Col() const noexcept { return this->c; }
//size_t Matrix::Size() const noexcept { return Row() * Col(); }
//
//Matrix Matrix::operator+(const Matrix& b) const {
//  assert(this->Row() == b.Row() && this->Col() == b.Col());
//
//  Matrix res(b.Row(), b.Col());
//  Elem e(0,0);
//  auto& [row, col] = e;
//  for(row = 0; row < b.Row(); ++row)
//    for(col = 0; col < b.Col(); ++col) res[e] = (*this)[e] + b[e];
//
//  return res;
//}
//Matrix Matrix::operator-(const Matrix& b) const {
//  assert(this->Row() == b.Row() && this->Col() == b.Col());
//  
//  Matrix res(b.Row(), b.Col());
//  Elem e(0,0);
//  auto& [row, col] = e;
//  for(row = 0; row < b.Row(); ++row)
//    for(col = 0; col < b.Col(); ++col) res[e] = (*this)[e] - b[e];
//
//  return res;
//}
//Matrix Matrix::operator*(const Matrix& b) const {
//  cout << this->Col() << ' ' << this->Row() << '\n';
//  assert(this->Col() == b.Row());
//  
//  Matrix result(this->Row(), b.Col());
//
//  Elem e;
//  auto& [row, col] = e;
//
//  for(row = 0; row < result.Row(); ++row) {
//    for(col = 0; col < result.Col(); ++col) {
//      result[e] = 0;
//      for(size_t index = 0; index < this->Col(); ++index) {
//        result[e] += ((*this)[{row, index}] * b[{index, col}]);
//      }
//    }
//  }
//  return result;
//}
//Matrix Matrix::operator*(double val) const {    
//  Matrix res((*this).Row(), (*this).Col());
//  Elem e(0,0);
//  auto& [row, col] = e;
//  for(row = 0; row < (*this).Row(); ++row)
//    for(col = 0; col < (*this).Col(); ++col) res[e] = (*this)[e] * val;
//
//  return res;
//}
//Matrix Matrix::operator/(double val) const {    
//  Matrix res((*this).Row(), (*this).Col());
//  Elem e(0,0);
//  auto& [row, col] = e;
//  for(row = 0; row < (*this).Row(); ++row)
//    for(col = 0; col < (*this).Col(); ++col) res[e] = (*this)[e] / val;
//
//  return res;
//}
//
//Matrix Matrix::Cut(Elem cutRowCol) const {
//  auto&[skipRow,skipCol]=cutRowCol;
//  --skipRow;
//  --skipCol;
//  assert(skipRow < Row() && skipCol < Col());
//
//  Matrix m(this->Row()-1, this->Col()-1);
//
//  size_t 
//    dr = 0,
//    dc = 0;
//
//  for(size_t row=0, dr=0 ; row < Row(); ++row){
//    if(row==skipRow) continue;
//    for(size_t col=0, dc = 0; col < Col(); ++col){
//      if(col==skipCol) continue;
//      m[{dr, dc}] = (*this)[{row, col}];
//      ++dc;
//    }
//    ++dr;
//  }
//  return m;
//}
//
//Matrix::~Matrix() {
//  this->Clean();
//}
//
//void Matrix::Swap(Matrix&& m) {
//  //this->data = m.data;
//  data.swap(m.data);
//  this->r = m.r;
//  this->c = m.c;
//
//  //m.data = nullptr;
//  m.r = 0;
//  m.c = 0;
//}
//void Matrix::Copy(const Data& data, size_t count) {
//  assert(this->Size() <= count);
//  //for(size_t i = 0; i < count; ++i)
//  // this->data[i] = data[i];
//  //memcpy(this->data, data, count * sizeof(double));
//  this->data = data;
//}
//void Matrix::Alloc(size_t r, size_t c) {
//  this->r = r;
//  this->c = c;
//
//  //this->data = new double[r*c];
//  data.resize(r * c);
//}
//void Matrix::Clean() {
//  //if(this->data) delete[] this->data;
//  //this->data = nullptr;
//  data.clear();
//  this->r = this->c = 0;
//}

#include "matrix.h"

