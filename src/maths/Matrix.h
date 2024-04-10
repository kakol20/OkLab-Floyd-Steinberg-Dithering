#pragma once

#include <vector>

/// <summary>
/// 1D Array Template
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
using Array1D = std::vector<T>;

/// <summary>
/// 2D Array Template
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
using Array2D = std::vector<std::vector<T>>;

class Matrix {
public:
    Matrix(const Array2D<double>& matrix);
    Matrix(const Matrix& other);
    Matrix(const size_t cols = 1, const size_t rows = 1);
    ~Matrix() {};

    // ----- ASSIGNMENT OPERATOR -----

    Matrix& operator=(const Array2D<double>& copy);
    Matrix& operator=(const Matrix& copy);

    // ----- MULTIPLICATION -----

    Matrix operator*(const Matrix& rhs) const;
    Matrix& operator*=(const Matrix& rhs);

    Matrix& operator/=(const double scalar);

    // ----- INVERSION -----

    /// <summary>
    /// Invert a 3 by 3 matrix
    /// </summary>
    /// <returns>If successful</returns>
    bool Invert3x3();

    /// <summary>
    /// Get Cofactor of a 3 by 3 matrix
    /// </summary>
    /// <returns>If successful</returns>
    bool Cofactor3x3();

    /// <summary>
    /// Get determinant of a 2 by 2 matrix
    /// </summary>
    /// <returns></returns>
    double Determinant2x2() const;

    void Transpose();

    /// <summary>
    /// Get determinant of a 3 by 3 matrix
    /// </summary>
    /// <returns></returns>
    double Determinant3x3() const;

    // ----- OTHER -----

    void Pow(const double pow);
    void Cbrt();

    inline double GetValue(const size_t col, const size_t row) const {
        return m_matrix[col][row];
    };

private:
    size_t m_cols, m_rows;

    Array2D<double> m_matrix;
};