#pragma once
#ifndef OBJECT3D_H
#define OBJECT3D_H
#include <windows.h>
#include <sstream>
#include <fstream>
#include <vector>
#include "Matrix.h"
class Object3D {
private:
	std::vector <Matrix<float>> vertexes_;
	std::vector <std::vector<int>> faces_;

public:
	std::vector <COLORREF> colors_faces_ = { 0x00a5ff };
	int line_width_ = 2;

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
	Object3D(const char* filename) {
		LoadObjModel(filename);
	}
	Object3D(std::vector<std::vector<float>> vertexes, std::vector <std::vector<int>> faces) {
		vertexes_.resize(vertexes.size());
		for (size_t i = 0; i < vertexes.size(); i++) {
			vertexes[i].push_back(1);
			vertexes_[i] = { vertexes[i] };
		}
		faces_ = faces;
	}

	void vertexesAndFaces(std::vector<std::vector<float>> vertexes, std::vector <std::vector<int>> faces) {
		vertexes_.clear(), faces_.clear();
		vertexes_.resize(vertexes.size());
		for (size_t i = 0; i < vertexes.size(); i++) {
			vertexes[i].push_back(1);
			vertexes_[i] = { vertexes[i] };
		}
		faces_ = faces;
	}
	void LoadObjModel(const char* filename) {
		vertexes_.clear(), faces_.clear();
		std::ifstream in(filename, std::ios::in);
		if (!in) exit(1);
		std::string line;
		while (getline(in, line)) {
			if (line.substr(0, 2) == "v ") {
				std::istringstream v(line.substr(2));
				float x, y, z;
				v >> x >> y >> z;
				Matrix <float> vertex({ {x, y, z, 1 } });
				vertexes_.push_back(vertex);
			}
			else if (line.substr(0, 2) == "f ") {
				faces_.resize(faces_.size() + 1);
				std::istringstream v(line.substr(2));
				std::string subline;
				while (v >> subline) {
					subline = subline.substr(0, subline.find("/"));
					faces_.back().push_back(atoi(subline.c_str()) - 1);
				}
			}
		}
		in.close();
	}

	void draw(Camera camera, HDC hdc) {
		for (size_t i = 0; i < faces_.size(); i++) {
			std::vector<Matrix<float>> face(faces_[i].size());
			for (size_t j = 0; j < face.size(); j++) {
				face[j] = vertexes_[faces_[i][j]];
				// перенос вершин объекта в пространство камеры + перенос вершин в пространство отсечения
				face[j] = face[j] * camera.cameraMatrix() * camera.projectionMatrix();
				for (int g = 0; g < 4; g++) {
					// нормализация вершин
					float temp = face[j](0, g) / face[j](0, 3);
					face[j](0, g, temp);
					// отбрасывание вершин за пределами отрисовки
					if (face[j](0, g) > 2 || face[j](0, g) < -2)
						face[i](0, g, 0);
				}
				// преобразование вершин в разрешение экрана
				face[j] = face[j] * camera.toScreenMatrix();
			}
			HPEN hPen;
			hPen = CreatePen(PS_DASHDOT, line_width_, colors_faces_.size() == 1 ? colors_faces_[0] : colors_faces_[i]);
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

class Axes : public Object3D {
public:
	Axes() : Object3D() {
		float size_face = 1;
		float size_normal = 0.2f;
		vertexesAndFaces({
			{0, 0, 0}, {size_normal, 0, 0}, {0, size_normal, 0}, {0, 0, size_normal},
			{size_face, 0, size_face}, {-size_face, 0, size_face}, {-size_face, 0, -size_face}, {size_face, 0, -size_face},
			{size_face, size_face, 0}, {-size_face, size_face, 0}, {-size_face, -size_face, 0}, {size_face, -size_face, 0},
			{0, size_face, size_face}, {0, -size_face, size_face}, {0, -size_face, -size_face}, {0, size_face, -size_face} },
			{ {0, 1}, {0, 2}, {0, 3},
			{4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15} });
		colors_faces_ = { 0xff0000, 0x00ff00, 0x0000ff, 0x00ff00, 0x0000ff, 0xff0000 };
		line_width_ = 2;
	}
};
#endif OBJECT3D_H