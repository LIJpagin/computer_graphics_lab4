#pragma once
#ifndef OBJECT3D_H
#define OBJECT3D_H
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include "Matrix.h"
class Object3D {
private:
	std::vector <Matrix<float>> vertexes_;
	std::vector <std::vector<float>> faces_;

public:
	bool movementPlay = false, drawVertexes = true;
	COLORREF color_faces_ = 0x00a5ff;

	Object3D() {
		std::vector <std::vector<float>> cube({
			{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}, 
			{0, 0, 1}, {0, 1, 1}, {1, 1, 1}, {1, 0, 1}
			});
		vertexes_.resize(cube.size());
		for (size_t i = 0; i < cube.size(); i++) {
			cube[i].push_back(1);
			vertexes_[i] = { cube[i] };
		}
		faces_ = { {0, 1, 2, 3}, {4, 5, 6, 7}, {0, 4, 5, 1}, {2, 3, 7, 6}, {1, 2, 6, 5}, {0, 3, 7, 4} };
	}
	Object3D(std::vector<std::vector<float>> vertexes, std::vector <std::vector<float>> faces) {
		vertexes_.resize(vertexes.size());
		for (size_t i = 0; i < vertexes.size(); i++) {
			vertexes[i].push_back(1);
			vertexes_[i] = { vertexes[i] };
		}
		faces_ = faces;
	}

	void vertexesAndFaces(std::vector<std::vector<float>> vertexes, std::vector <std::vector<float>> faces) {
		vertexes_.clear(), faces_.clear();
		vertexes_.resize(vertexes.size());
		for (size_t i = 0; i < vertexes.size(); i++) {
			vertexes[i].push_back(1);
			vertexes_[i] = { vertexes[i] };
		}
		faces_ = faces;
	}
	/*void LoadObjModel(const char* filename) {
		std::ifstream in(filename, std::ios::in);
		if (!in) exit(1);
		std::string line;
		while (std::getline(in, line)) {
			if (line.substr(0, 2) == "v ") {
				std::istringstream v(line.substr(2));
				Matrix <float> vertex(1, 4);
				v >> vertex(0, 0) >> vertex(0, 1) >> vertex(0, 2);
				vertexes_.push_back(vertex);
			}
			else if (line.substr(0, 2) == "f ") {
				int a, b, c;
				int A, B, C;
				const char* chh = line.c_str();
				sscanf(chh, "f %i/%i %i/%i %i/%i", &a, &A, &b, &B, &c, &C);
				a--; b--; c--;
				faceIndex.push_back(a);
				faceIndex.push_back(b);
				faceIndex.push_back(c);
			}

		}
	}*/

	void draw(Camera camera, HDC hdc) {
		screenProjection(camera, hdc);
		movement();
	}
	void movement() {
		srand((unsigned int)time(NULL));
		if (movementPlay) {
			rotate((float)(rand() % 51 - 25) / 100, (float)(rand() % 51 - 25) / 100, (float)(rand() % 51 - 25) / 100);
			float value = 1.0f + (float)(rand() % 5 - 2) / 1000;
			scale(value, value, value);
		}
	}
	void screenProjection(Camera camera, HDC hdc) {
		for (size_t i = 0; i < faces_.size(); i++) {
			std::vector<Matrix<float>> face(faces_[i].size());
			for (size_t j = 0; j < face.size(); j++) {
				face[j] = vertexes_[faces_[i][j]];
				// перенос вершин объекта в пространство камеры + перенос вершин в пространство отсечения
				face[j] = face[j] * camera.cameraMatrix() * camera.projectionMatrix();
				for (size_t g = 0; g < 4; g++) {
					// нормализация вершин
					face[j](0, g, (face[j](0, g) / face[j](0, 3)));
					// отбрасывание вершин за пределами отрисовки
					if (face[j](0, g) > 2 || face[j](0, g) < -2)
						face[i](0, g, 0);
				}
				// преобразование вершин в разрешение экрана
				face[j] = face[j] * camera.toScreenMatrix();
			}
			HPEN hPen;
			hPen = CreatePen(PS_DASHDOT, 2, color_faces_);
			SelectObject(hdc, hPen);
			MoveToEx(hdc, (int)round(face[0](0, 0)), (int)round(face[0](0, 1)), NULL);
			for (size_t i = 1; i < face.size(); i++)
				LineTo(hdc, (int)round(face[i](0, 0)), (int)round(face[i](0, 1)));
			LineTo(hdc, (int)round(face[0](0, 0)), (int)round(face[0](0, 1)));
			DeleteObject(hPen);
		}
	}

	void translate(float Tx, float Ty, float Tz) {
		for (size_t i = 0; i < vertexes_.size(); i++)
			vertexes_[i] = vertexes_[i] * Matrix<float>::transfer(Tx, Ty, Tz);
	}
	void scale(float Sx, float Sy, float Sz) {
		for (size_t i = 0; i < vertexes_.size(); i++)
			vertexes_[i] = vertexes_[i] * Matrix<float>::scaling(Sx, Sy, Sz);
	}
	void rotate(float Rx, float Ry, float Rz) {
		for (size_t i = 0; i < vertexes_.size(); i++)
			vertexes_[i] = vertexes_[i] * Matrix<float>::rotation(Rx, Ry, Rz);
	}

	~Object3D() { }
};
#endif OBJECT3D_H