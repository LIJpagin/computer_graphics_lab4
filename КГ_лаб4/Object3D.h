#pragma once

#include <windows.h>
#include <sstream>
#include <fstream>
#include <vector>

#include "Matrix.h"
#include "Camera.h"
#include "Light.h"
#include "Fill.h"

#ifndef OBJECT3D_H
#define OBJECT3D_H

class Object3D {
private:
	std::vector <Matrix<float>> vertices_;
	std::vector <std::vector<int>> faces_;
	std::vector <Matrix<float>> normals_;

protected:
	Matrix<float> calculateNormal(std::vector<Matrix<float>> face) {
		Matrix<float> normal(1, 4);
		normal.w(0);
		if (face.size() < 3) return normal;
		Matrix<float> v1 = face[2], v2 = face[1], v3 = face[0];
		normal.x((v2.y() - v1.y()) * (v3.z() - v1.z()) - (v2.z() - v1.z()) * (v3.y() - v1.y()));
		normal.y((v2.z() - v1.z()) * (v3.x() - v1.x()) - (v2.x() - v1.x()) * (v3.z() - v1.z()));
		normal.z((v2.x() - v1.x()) * (v3.y() - v1.y()) - (v2.y() - v1.y()) * (v3.x() - v1.x()));
		return normal;
	}
	std::vector<Matrix<int>> translatingFaceIntoScreenSpace(Camera camera, std::vector<Matrix<float>> face) {
		std::vector<Matrix<int>> face_into_screen_space(face.size());
		for (size_t j = 0; j < face.size(); j++) {
			// перенос вершин объекта в пространство камеры + перенос вершин в пространство отсечени€
			face[j] = face[j] * camera.cameraMatrix() * camera.projectionMatrix();
			for (int g = 0; g < 4; g++) {
				// нормализаци€ вершин
				face[j](0, g, face[j](0, g) / face[j].w());
				// отбрасывание вершин за пределами отрисовки
				//if (face[j](0, g) > 1 || face[j](0, g) < -1)
				//	face[j](0, g, 0);
			}
			// преобразование вершин в разрешение экрана
			face[j] = face[j] * camera.toScreenMatrix();
			int x = (int)round(face[j].x()), y = (int)round(face[j].y());
			if (x > 1600 || y > 900 || x < 0 || y < 0)
				x = 0, y = 0;
			face_into_screen_space[j] = { {x, y} };
		}
		return face_into_screen_space;
	}
	bool robertsAlgorithm(std::vector<Matrix<int>> face) {
		// јлгоритм –обертса провер€ет сонаправленность нормали и вектора взгл€да.
		// ≈сли все грани заданы одним направлением обхода, то проверив направление
		// обхода дл€ спроецированной грани, имеем факт сонаправленности нормали
		// и вектора взгл€да
		int sum = 0;
		for (size_t j = 0; j < face.size(); j++)
			sum += (face[j].x() - face[j == face.size() - 1 ? 0 : j + 1].x())
			* (face[j].y() + face[j == face.size() - 1 ? 0 : j + 1].y());
		return sum < 0 ? true : false;
	}

public:
	std::vector <COLORREF> colors_faces_ = { 0xA7FC00 };
	int line_width_ = 2;

	Object3D() { }
	Object3D(const char* filename) {
		LoadObjModel(filename);
	}
	Object3D(std::vector<std::vector<float>> vertices, std::vector<std::vector<int>> faces) {
		vertices_.resize(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i].push_back(1);
			vertices_[i] = { vertices[i] };
		}
		faces_ = faces;
	}
	Object3D(std::vector<Matrix<float>> vertices, std::vector<std::vector<int>> faces) {
		vertices_ = vertices, faces_ = faces;
	}

	void cube() {
		std::vector <std::vector<float>> cube({
			{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f},
			{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
			{-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}
			});
		vertices_.resize(cube.size());
		for (size_t i = 0; i < cube.size(); i++) {
			cube[i].push_back(1);
			vertices_[i] = { cube[i] };
		}
		faces_ = {
			{0, 1, 3, 2}, {2, 3, 5, 4},
			{4, 5, 7, 6}, {6, 7, 1, 0},
			{1, 7, 5, 3}, {6, 0, 2, 4}, };
		colors_faces_ = { 0xff0000, 0x00ff00, 0x0000ff, 0x00ffff, 0xffff00, 0xff00ff };
		normals_.resize(faces_.size());
	}
	void vertexesAndFaces(std::vector<std::vector<float>> vertexes, std::vector<std::vector<int>> faces) {
		vertices_.clear(), faces_.clear();
		vertices_.resize(vertexes.size());
		for (size_t i = 0; i < vertexes.size(); i++) {
			vertexes[i].push_back(1);
			vertices_[i] = { vertexes[i] };
		}
		faces_ = faces;
	}
	void vertexesAndFaces(std::vector<Matrix<float>> vertexes, std::vector<std::vector<int>> faces) {
		vertices_.clear(), faces_.clear();
		vertices_ = vertices_ = vertexes;
		faces_ = faces;
	}
	void LoadObjModel(const char* filename) {
		vertices_.clear(), faces_.clear();
		std::ifstream in(filename, std::ios::in);
		if (!in) exit(1);
		std::string line;
		while (getline(in, line)) {
			if (line.substr(0, 2) == "v ") {
				std::istringstream v(line.substr(2));
				float x, y, z;
				v >> x >> y >> z;
				Matrix <float> vertex({ {x, y, z, 1 } });
				vertices_.push_back(vertex);
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
		normals_.resize(faces_.size());
	}

	void drawLambertPolygonFill(Camera camera, std::vector<Light> light_sources, HDC hdc) {
		for (size_t i = 0; i < faces_.size(); i++) {

			std::vector<Matrix<float>> face(faces_[i].size());
			for (size_t j = 0; j < face.size(); j++)
				face[j] = vertices_[faces_[i][j]];

			std::vector<Matrix<int>> face_into_screen_space
				= translatingFaceIntoScreenSpace(camera, face);

			normals_[i] = calculateNormal(face);

			if (!robertsAlgorithm(face_into_screen_space)) continue;

			float intensity = 0;
			for (auto light_itr : light_sources)
				intensity += light_itr.faceLightIntensity(normals_[i], face);
			if (intensity > 1) intensity = 1;

			COLORREF color = colors_faces_.size() == 1 ? colors_faces_[0] : colors_faces_[i];
			color = RGB(intensity * GetRValue(color), intensity * GetGValue(color),
				intensity * GetBValue(color));

			POINT* face_into_POINT = new POINT[face_into_screen_space.size()];
			for (size_t i = 0; i < face_into_screen_space.size(); i++)
				face_into_POINT[i] = { face_into_screen_space[i].x(), face_into_screen_space[i].y() };

			HBRUSH hBrush = CreateSolidBrush(color);
			HPEN hPen = CreatePen(PS_DASHDOT, 1, color);
			SetBkColor(hdc, color);
			SelectObject(hdc, hBrush);
			SelectObject(hdc, hPen);
			Polygon(hdc, face_into_POINT, face_into_screen_space.size());
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}
	}
	void drawLambert(Camera camera, std::vector<Light> light_sources, HDC hdc) {
		for (size_t i = 0; i < faces_.size(); i++) {

			std::vector<Matrix<float>> face(faces_[i].size());
			for (size_t j = 0; j < face.size(); j++)
				face[j] = vertices_[faces_[i][j]];

			std::vector<Matrix<int>> face_into_screen_space
				= translatingFaceIntoScreenSpace(camera, face);

			normals_[i] = calculateNormal(face);

			if (!robertsAlgorithm(face_into_screen_space)) continue;

			float intensity = 0;
			for (auto light_itr : light_sources)
				intensity += light_itr.faceLightIntensity(normals_[i], face);
			if (intensity > 1) intensity = 1;

			COLORREF color = colors_faces_.size() == 1 ? colors_faces_[0] : colors_faces_[i];
			color = RGB(intensity * GetRValue(color), intensity * GetGValue(color),
				intensity * GetBValue(color));

			Fill fill_face(camera.getHeightScreen());
			fill_face.fillLines(hdc, face_into_screen_space, color);
		}
	}
	void drawFill(Camera camera, HDC hdc) {
		for (size_t i = 0; i < faces_.size(); i++) {
			std::vector<Matrix<float>> face(faces_[i].size());
			for (size_t j = 0; j < face.size(); j++)
				face[j] = vertices_[faces_[i][j]];

			std::vector<Matrix<int>> face_into_screen_space = translatingFaceIntoScreenSpace(camera, face);

			if (!robertsAlgorithm(face_into_screen_space)) continue;

			COLORREF color = colors_faces_.size() == 1 ? colors_faces_[0] : colors_faces_[i];
			Fill fill_face(camera.getHeightScreen());
			fill_face.fillLines(hdc, face_into_screen_space, color);

			HPEN hPen;
			SetBkColor(hdc, color - 0xffffff);
			hPen = CreatePen(PS_DASHDOT, 1, color - 0xffffff);
			SelectObject(hdc, hPen);
			for (size_t j = 0; j < face_into_screen_space.size(); j++) {
				size_t g = j == face_into_screen_space.size() - 1 ? 0 : j + 1;
				if (face_into_screen_space[j].x() <= 0 || face_into_screen_space[j].y() >= camera.getHeightScreen() ||
					face_into_screen_space[g].x() <= 0 || face_into_screen_space[g].y() >= camera.getHeightScreen())
					continue;
				MoveToEx(hdc, face_into_screen_space[j].x(), face_into_screen_space[j].y(), NULL);
				LineTo(hdc, face_into_screen_space[g].x(), face_into_screen_space[g].y());
			}
			DeleteObject(hPen);
		}
	}
	void drawFrame(Camera camera, HDC hdc) {
		for (size_t i = 0; i < faces_.size(); i++) {
			std::vector<Matrix<float>> face(faces_[i].size());
			for (size_t j = 0; j < face.size(); j++)
				face[j] = vertices_[faces_[i][j]];

			std::vector<Matrix<int>> face_into_screen_space = translatingFaceIntoScreenSpace(camera, face);

			HPEN hPen;
			SetBkColor(hdc, colors_faces_.size() == 1 ? colors_faces_[0] : colors_faces_[i]);
			hPen = CreatePen(PS_DASHDOT, 1, colors_faces_.size() == 1 ? colors_faces_[0] : colors_faces_[i]);
			SelectObject(hdc, hPen);
			for (size_t j = 0; j < face_into_screen_space.size(); j++) {
				size_t g = j == face_into_screen_space.size() - 1 ? 0 : j + 1;
				if (face_into_screen_space[j].x() <= 0 || face_into_screen_space[j].y() >= camera.getHeightScreen() ||
					face_into_screen_space[g].x() <= 0 || face_into_screen_space[g].y() >= camera.getHeightScreen())
					continue;
				MoveToEx(hdc, face_into_screen_space[j].x(), face_into_screen_space[j].y(), NULL);
				LineTo(hdc, face_into_screen_space[g].x(), face_into_screen_space[g].y());
			}
			DeleteObject(hPen);
		}
	}
	void drawVertices(Camera camera, HDC hdc) {
		for (size_t i = 0; i < faces_.size(); i++) {
			std::vector<Matrix<float>> face(faces_[i].size());
			for (size_t j = 0; j < face.size(); j++)
				face[j] = vertices_[faces_[i][j]];

			std::vector<Matrix<int>> face_into_screen_space
				= translatingFaceIntoScreenSpace(camera, face);

			HPEN hPen;
			COLORREF color = colors_faces_.size() == 1 ? colors_faces_[0] : colors_faces_[i];
			SetBkColor(hdc, color);
			hPen = CreatePen(PS_DASHDOT, 1, color);
			SelectObject(hdc, hPen);
			for (auto face_itr : face_into_screen_space)
				SetPixel(hdc, face_itr.x(), face_itr.y(), color);
			DeleteObject(hPen);
		}
	}

	void translate(float Tx, float Ty, float Tz) {
		for (size_t i = 0; i < vertices_.size(); i++)
			vertices_[i] = vertices_[i] * Matrix<float>::transfer(Tx, Ty, Tz);
	}
	void scale(float Sx, float Sy, float Sz) {
		for (size_t i = 0; i < vertices_.size(); i++)
			vertices_[i] = vertices_[i] * Matrix<float>::scaling(Sx, Sy, Sz);
	}
	void rotate(float Rx, float Ry, float Rz) {
		for (size_t i = 0; i < vertices_.size(); i++)
			vertices_[i] = vertices_[i] * Matrix<float>::rotation(Rx, Ry, Rz);
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
	~Axes() { }
};

#endif OBJECT3D_H