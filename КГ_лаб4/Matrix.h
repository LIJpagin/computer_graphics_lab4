#pragma once

#define PI (float)3.14159

#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>

template <typename Type>
class Matrix {
private:
    std::vector <std::vector<Type>> matrix_;
    int row_, col_;

public:
    Matrix() : row_(0), col_(0) { }
    Matrix(int row, int col) {
        row_ = row, col_ = col;
        matrix_.resize(row_, std::vector<Type>(col_, 0));
    }
    Matrix(const Matrix& that) {
        row_ = that.row_, col_ = that.col_;
        matrix_ = that.matrix_;
    }
    Matrix(std::vector <std::vector<Type>> matrix) {
        row_ = (int)matrix.size(), col_ = (int)matrix[0].size();
        matrix_ = matrix;
    }

    Type x() { return matrix_[0][0]; }
    Type y() { return matrix_[0][1]; }
    Type z() { return matrix_[0][2]; }
    Type w() { return matrix_[0].back(); }
    void x(Type value) { matrix_[0][0] = value; }
    void y(Type value) { matrix_[0][1] = value; }
    void z(Type value) { matrix_[0][2] = value; }
    void w(Type value) { matrix_[0].back() = value; }

    Type operator() (int row, int col) {
        if (row < 0 || col < 0 || row > row_ - 1 || col > col_ - 1)
            throw new std::exception("");
        return matrix_[row][col];
    }
    void operator() (int row, int col, float value) {
        if (row < 0 || col < 0 || row > row_ - 1 || col > col_ - 1)
            throw new std::exception("");
        matrix_[row][col] = value;
    }
    friend std::ostream& operator<< (std::ostream& out, const Matrix& matrix) {
        if (!matrix.row_ && !matrix.col_) {
            out << "Matrix is empty" << std::endl;
            return out;
        }
        for (int i = 0; i < matrix.row_; i++) {
            for (int j = 0; j < matrix.col_; j++)
                out << matrix.matrix_[i][j] << "\t";
            out << std::endl;
        }
        return out;
    }
    friend Matrix <Type> operator* (const Matrix& left, const Matrix& right) {
        if (left.col_ != right.row_)
            throw new std::exception("");
        Matrix <Type> result(left.row_, right.col_);
        for (auto row = 0; row < left.row_; row++)
            for (auto col = 0; col < right.col_; col++)
                for (auto inner = 0; inner < left.col_; inner++)
                    result.matrix_[row][col] += left.matrix_[row][inner] * right.matrix_[inner][col];
        return result;
    }
    friend Matrix <Type> operator* (const Matrix& left, const Type& value) {
        Matrix <Type> result(left);
        for (auto row = 0; row < result.row_; row++)
            for (auto col = 0; col < result.col_; col++)
                result.matrix_[row][col] = left.matrix_[row][col] * value;
        return result;
    }
    friend Matrix <Type> operator+ (const Matrix& left, const Matrix& right) {
        if (left.row_ != right.row_ || left.col_ != right.col_)
            throw new std::exception("");
        Matrix <Type> result(left.row_, left.col_);
        for (auto row = 0; row < result.row_; row++)
            for (auto col = 0; col < result.col_; col++)
                 result.matrix_[row][col] = left.matrix_[row][col] + right.matrix_[row][col];
        return result;
    }
    friend Matrix <Type> operator/ (const Matrix& left, const Type& value) {
        Matrix <Type> result(left);
        for (auto row = 0; row < result.row_; row++)
            for (auto col = 0; col < result.col_; col++)
                result.matrix_[row][col] = left.matrix_[row][col] / value;
        return result;
    }
    friend Matrix <Type> operator- (const Matrix& left, const Matrix& right) {
        if (left.row_ != right.row_ || left.col_ != right.col_)
            throw new std::exception("");
        Matrix <Type> result(left.row_, left.col_);
        for (auto row = 0; row < result.row_; row++)
            for (auto col = 0; col < result.col_; col++)
                result.matrix_[row][col] = left.matrix_[row][col] - right.matrix_[row][col];
        return result;
    }
    Matrix operator= (const Matrix& that) {
        if (&that == this) return *this;
        this->row_ = that.row_, this->col_ = that.col_;
        this->matrix_.clear();
        this->matrix_ = that.matrix_;
        return *this;
    }
    Matrix operator= (std::vector <std::vector<Type>> matrix) {
        matrix_.clear();
        row_ = (int)matrix.size(), col_ = (int)matrix[0].size();
        matrix_ = matrix;
        return *this;
    }

    ~Matrix() { }

    static Matrix <float> transfer(float Tx, float Ty, float Tz) {
        Matrix <float> translate({
            { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { Tx, Ty, Tz, 1 }
            });
        return translate;
    }
    static Matrix <float> scaling(float Sx, float Sy, float Sz) {
        Matrix <float> scale({
            { Sx, 0, 0, 0 },
            { 0, Sy, 0, 0 },
            { 0, 0, Sz, 0 },
            { 0, 0, 0, 1 }
            });
        return scale;
    }
    static Matrix <float> rotation(float Rx, float Ry, float Rz) {
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
};
#endif MATRIX_H