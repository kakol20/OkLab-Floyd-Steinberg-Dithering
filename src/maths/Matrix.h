#pragma once

#include <string>

#include "../other/Pseudo2DArray.hpp"

class Matrix {
public:
  Matrix(const std::vector<long double>& arr = { 1 }, const unsigned int cols = 1, const unsigned int rows = 1);
  Matrix(const Pseudo2DArray<long double>& arr);
  Matrix(const Matrix& other);
  ~Matrix() {};

  Matrix& operator=(const Matrix& other);

  Matrix& operator*=(const Matrix& rhs);
  Matrix operator*(const Matrix& rhs) const;

  Matrix& operator*=(const long double scalar);
  Matrix operator*(const long double scalar) const;

  long double& operator()(const unsigned int x, const unsigned int y);
  long double operator()(const unsigned int x, const unsigned int y) const;

  void Pow(const long double p);
  void Cbrt();
  void NRoot(const long double n);

  long double Determinant3x3() const;
  void Transpose();
  long double Determinant2x2() const;
  bool Cofactor3x3();
  bool Invert3x3();

  std::string Debug(const int indentSize = 0) const { return m_mat.Debug(indentSize); };

private:
  Pseudo2DArray<long double> m_mat;

  unsigned int m_cols, m_rows;
};