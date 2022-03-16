#pragma once
#define PI 3.14159
#include <vector>
#include <iostream>
#ifndef MATRIX_H
#define MATRIX_H
template <typename Type>
class Matrix {
private:
    std::vector <std::vector<Type>> matrix_;
    int row_, col_;

public:
    Matrix() : row_(0), col_(0) { }
    Matrix(int m, int n) {
        row_ = m, col_ = n;
        matrix_.resize(m, std::vector<Type>(n, 0));
    }
    Matrix(const Matrix& that) {
        row_ = that.row_, col_ = that.col_;
        matrix_ = that.matrix_;
    }
    Matrix(std::vector <std::vector<Type>> matrix) {
        row_ = matrix.size(), col_ = matrix[0].size();
        matrix_ = matrix;
    }
    void print() {
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < col_; j++)
                std::cout << matrix_[i][j] << "\t";
            std::cout << std::endl;
        }
    }
    friend std::ostream& operator<< (std::ostream& out, const Matrix& matrix) {
        for (int i = 0; i < matrix.row_; i++) {
            for (int j = 0; j < matrix.col_; j++)
                out << matrix.matrix_[i][j] << "\t";
            out << std::endl;
        }
        return out;
    }
    friend Matrix <Type> operator* (const Matrix& left, const Matrix& right) {
        Matrix <Type> result(left.row_, right.col_);
        for (auto row = 0; row < left.row_; row++)
            for (auto col = 0; col < right.col_; col++)
                for (auto inner = 0; inner < left.col_; inner++)
                    result.matrix_[row][col] += left.matrix_[row][inner] * right.matrix_[inner][col];
        return result;
    }
    Matrix operator= (const Matrix& that) {
        if (that == this) return *this;
        this->row_ = that.row_, this->col_ = that.col_;
        this->matrix_.clear();
        this->matrix_ = that.matrix_;
        return *this;
    }
    ~Matrix() { }
};

Matrix <float> transfer(float tx, float ty, float tz) {
    Matrix <float> translate({
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { tx, ty, tz, 1 }
        });
    return translate;
}
Matrix <float> scaling(float Sx, float Sy, float Sz) {
    Matrix <float> scale({
        { Sx, 0, 0, 0 },
        { 0, Sy, 0, 0 },
        { 0, 0, Sz, 0 },
        { 0, 0, 0, 1 }
        });
    return scale;
}
Matrix <float> rotation(float Rx, float Ry, float Rz) {
    Rx = Rx * PI / 180, Ry = Ry * PI / 180, Rz = Rz * PI / 180;
    Matrix <float> rotateX({
        { 1, 0, 0, 0 },
        { 0, cos(Rx), sin(Rx), 0 },
        { 0, -sin(Rx), cos(Rx), 0 },
        { 0, 0, 0, 1 }
        });
    Matrix <float> rotateY({
        { cos(Ry), 0, -sin(Ry), 0 },
        { 0, 1, 0, 0 },
        { sin(Ry), 0, cos(Ry), 0 },
        { 0, 0, 0, 1 }
        });
    Matrix <float> rotateZ({
        { cos(Rz), sin(Rz), 0, 0 },
        { -sin(Rz), cos(Rz), 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
        });
    return (rotateX * rotateY * rotateZ);
}
#endif MATRIX_H