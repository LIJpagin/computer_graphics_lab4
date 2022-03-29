#pragma once

#include <windows.h>
#include <vector>
#include "Matrix.h"

#ifndef CAMERA_H
#define CAMERA_H

class Camera {
private:
    Matrix <float> position_, forward_, up_, right_;
    int width_ = 1600, height_ = 800;
    float v_fov_, h_fov_ = PI / 3;
    float near_plane_ = 0.1f, far_plane_ = 100;


public:
	Camera(int width, int height, std::vector<float> position) {
        if (width > 100 || height > 100)
            width_ = width, height_ = height;
        v_fov_ = h_fov_ * ((float)height_ / width_);
        position.push_back(1);
        position_ = { position };
        forward_ =   { {0, 0, 1, 1} };
        up_ =        { {0, 1, 0, 1} };
        right_ =     { {1, 0, 0, 1} };
	}

    int getWidthScreen() {
        return width_;
    }
    int getHeightScreen() {
        return height_;
    }

    Matrix <float> cameraMatrix() {
        float x = position_(0, 0), y = position_(0, 1), z = position_(0, 2);
        Matrix<float> transfer = Matrix<float>::transfer(-x, -y, -z);
        transfer(1, 3, 1);
        Matrix <float> rotation({
            {right_(0, 0), up_(0, 0), forward_(0, 0), 0},
            {right_(0, 1), up_(0, 1), forward_(0, 1), 0},
            {right_(0, 2), up_(0, 2), forward_(0, 2), 0},
            {0, 0, 0, 1}
            });
        return (transfer * rotation);
    }
    Matrix <float> projectionMatrix() {
        float m00 = 1 / tan(h_fov_ / 2);
        float m11 = 1 / tan(v_fov_ / 2);
        float m22 = (far_plane_ + near_plane_) / (far_plane_ - near_plane_);
        float m32 = (-2 * near_plane_ * far_plane_) / (far_plane_ - near_plane_);
        Matrix <float> projection_matrix({
            {m00, 0, 0, 0},
            {0, m11, 0, 0},
            {0, 0, m22, 1},
            {0, 0, m32, 0}
            });
        return projection_matrix;
    }
    Matrix <float> toScreenMatrix() {
        float HW = (float)(width_ / 2), HH = (float)(height_ / 2);
        Matrix <float> to_screen_matrix({
            {HW,  0, 0, 0},
            {0, -HH, 0, 0},
            {0,   0, 1, 0},
            {HW, HH, 0, 1}
            });
        return to_screen_matrix;
    }

    void translate(float Tx, float Ty, float Tz) {
        position_ = position_ + forward_ * Tx;
        position_ = position_ + right_ * Ty;
        position_ = position_ + up_ * Tz;
    }
    void rotate(float Rx, float Ry, float Rz) {
        forward_ = forward_ * Matrix<float>::rotation(Rx, Ry, Rz);
        right_ = right_ * Matrix<float>::rotation(Rx, Ry, Rz);
        up_ = up_ * Matrix<float>::rotation(Rx, Ry, Rz);
    }

	~Camera() { }
};

#endif CAMERA_H
