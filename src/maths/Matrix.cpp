#include <cmath>

#include "Matrix.h"

//Matrix::Matrix(const size_t cols, const size_t rows) {
//}

Matrix::Matrix(const Array2D<double>& matrix) {
    m_cols = matrix.size();
    m_rows = matrix[0].size();

    m_matrix.clear();
    m_matrix.reserve(m_cols);

    for (size_t i = 0; i < m_cols; i++) {
        Array1D<double> jGrid;
        jGrid.reserve(m_rows);

        for (size_t j = 0; j < m_rows; j++) {
            jGrid.push_back(matrix[i][j]);
        }
        m_matrix.push_back(jGrid);
    }
}

Matrix::Matrix(const Matrix& other) {
    m_cols = other.m_matrix.size();
    m_rows = other.m_matrix[0].size();

    m_matrix.clear();
    m_matrix.reserve(m_cols);

    for (size_t i = 0; i < m_cols; i++) {
        Array1D<double> jGrid;
        jGrid.reserve(m_rows);

        for (size_t j = 0; j < m_rows; j++) {
            jGrid.push_back(other.m_matrix[i][j]);
        }
        m_matrix.push_back(jGrid);
    }
}

Matrix::Matrix(const size_t cols, const size_t rows) {
    m_cols = cols;
    m_rows = rows;

    m_matrix.clear();
    m_matrix.reserve(m_cols);

    for (size_t i = 0; i < m_cols; i++) {
        Array1D<double> jGrid;
        jGrid.reserve(m_rows);

        for (size_t j = 0; j < m_rows; j++) {
            jGrid.push_back(0.0);
        }
        m_matrix.push_back(jGrid);
    }
}

Matrix& Matrix::operator=(const Array2D<double>& copy) {
    m_cols = copy.size();
    m_rows = copy[0].size();

    m_matrix.clear();
    m_matrix.reserve(m_cols);

    for (size_t i = 0; i < m_cols; i++) {
        Array1D<double> jGrid;
        jGrid.reserve(m_rows);

        for (size_t j = 0; j < m_rows; j++) {
            jGrid.push_back(copy[i][j]);
        }
        m_matrix.push_back(jGrid);
    }
    return *this;
}

Matrix& Matrix::operator=(const Matrix& copy) {
    if (this == &copy) return *this;

    m_cols = copy.m_matrix.size();
    m_rows = copy.m_matrix[0].size();

    m_matrix.clear();
    m_matrix.reserve(m_cols);

    for (size_t i = 0; i < m_cols; i++) {
        Array1D<double> jGrid;
        jGrid.reserve(m_rows);

        for (size_t j = 0; j < m_rows; j++) {
            jGrid.push_back(copy.m_matrix[i][j]);
        }
        m_matrix.push_back(jGrid);
    }

    return *this;
}

Matrix Matrix::operator*(const Matrix& rhs) const {
    Matrix out(m_matrix);
    out *= rhs;
    return out;
}

Matrix& Matrix::operator*=(const Matrix& rhs) {
    if (m_cols != rhs.m_rows) return *this; // Invalid multiplication

    const size_t newCols = rhs.m_cols;
    const size_t newRows = m_rows;

    Array2D<double> newMat;
    newMat.reserve(newCols);
    for (size_t i = 0; i < newCols; i++) {
        Array1D<double> jGrid;
        jGrid.reserve(newRows);
        for (size_t j = 0; j < newRows; j++) {
            double total = 0;

            for (size_t k = 0; k < rhs.m_rows; k++) {
                total += m_matrix[k][j] * rhs.m_matrix[i][k];
            }
            jGrid.push_back(total);
        }
        newMat.push_back(jGrid);
    }

    (*this) = newMat;
    return *this;
}

Matrix& Matrix::operator/=(const double scalar) {
    for (size_t i = 0; i < m_cols; i++) {
        for (size_t j = 0; j < m_rows; j++) {
            m_matrix[i][j] /= scalar;
        }
    }

    return *this;
}

bool Matrix::Invert3x3() {
    // https://www.cuemath.com/algebra/inverse-of-3x3-matrix/
    if (m_cols == 3 && m_rows == 3) {
        // Find Adjoint of matrix
        Matrix adjoint(m_matrix);
        adjoint.Cofactor3x3();
        adjoint.Transpose();

        const double determinant = (*this).Determinant3x3();

        if (determinant == 0.0) return false;

        adjoint /= determinant;

        (*this) = adjoint;

        return true;
    }

    return false;
}

bool Matrix::Cofactor3x3() {
    if (m_cols == 3 && m_rows == 3) {
        Matrix newMat(3, 3);

        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 3; j++) {
                const size_t colMax = i <= 1 ? 2 : 1;
                const size_t colMin = i >= 1 ? 0 : 1;

                const size_t rowMax = j <= 1 ? 2 : 1;
                const size_t rowMin = j >= 1 ? 0 : 1;

                Array2D<double> detArr;
                detArr.reserve(2);
                detArr.push_back({ m_matrix[colMin][rowMin], m_matrix[colMin][rowMax] });
                detArr.push_back({ m_matrix[colMax][rowMin], m_matrix[colMax][rowMax] });

                Matrix detMat = detArr;

                newMat.m_matrix[i][j] = detMat.Determinant2x2() * std::pow(-1, i + j + 2);
            }
        }

        (*this) = newMat;

        return true;
    }
    return false;
}

double Matrix::Determinant2x2() const {
    if (m_cols == 2 && m_rows == 2) {
        return (m_matrix[0][0] * m_matrix[1][1]) - (m_matrix[1][0] * m_matrix[0][1]);
    }
    return NAN;
}

void Matrix::Transpose() {
    Matrix newMat(m_cols, m_rows);

    for (size_t i = 0; i < m_cols; i++) {
        for (size_t j = 0; j < m_rows; j++) {
            newMat.m_matrix[j][i] = m_matrix[i][j];
        }
    }

    (*this) = newMat;
}

double Matrix::Determinant3x3() const {
    if (m_cols == 3 && m_rows == 3) {
        return (m_matrix[0][0] * m_matrix[1][1] * m_matrix[2][2]) +
            (m_matrix[1][0] * m_matrix[2][1] * m_matrix[0][2]) +
            (m_matrix[2][0] * m_matrix[0][1] * m_matrix[1][2]) -

            (m_matrix[0][0] * m_matrix[2][1] * m_matrix[1][2]) -
            (m_matrix[1][0] * m_matrix[0][1] * m_matrix[2][2]) -
            (m_matrix[2][0] * m_matrix[1][1] * m_matrix[0][2]);
    }

    return NAN;
}

void Matrix::Pow(const double pow) {
    for (size_t i = 0; i < m_cols; i++) {
        for (size_t j = 0; j < m_rows; j++) {
            m_matrix[i][j] = std::pow(m_matrix[i][j], pow);
        }
    }
}

void Matrix::Cbrt() {
    for (size_t i = 0; i < m_cols; i++) {
        for (size_t j = 0; j < m_rows; j++) {
            m_matrix[i][j] = std::cbrt(m_matrix[i][j]);
        }
    }
}