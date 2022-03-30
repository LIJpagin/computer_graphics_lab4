#pragma once

#include <vector>
#include "Matrix.h"
#include "Object3D.h"

#ifndef LIGHT_H
#define LIGHT_H

class Light {
private:
	Matrix<float> vector_light_;
	Matrix<float> position_;
	float cosineVectors(Matrix<float> v1, Matrix<float> v2) {
		double cosine = (v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z())
			/ (sqrt(pow(v1.x(), 2) + pow(v1.y(), 2) + pow(v1.z(), 2))
			* sqrt(pow(v2.x(), 2) + pow(v2.y(), 2) + pow(v2.z(), 2)));
		return (float)cosine;
	}
	float distanceBetweenVertices(Matrix<float> v1, Matrix<float> v2) {
		return (float)sqrt(pow(v1.x() - v2.x(), 2) + pow(v1.y() - v2.y(), 2) + pow(v1.z() - v2.z(), 2));
	}

public:
	float intensity_light_source = 1000;
	float coeff_diffuse_reflection_light_source = 0.9f;
	float intensity_scattered_light = 50;
	float coef_diffuse_reflection_scattered_light = 0.4f;
	float coeff_for_distance = 5;
	float coeff_reflection_curve = 1;
	int n = 60;

	Light() {
		vector_light_ = { {-1, 1, -1, 0} };
		position_ = { {0, 0, 0, 1} };
	}
	Light(std::vector<float> vec, std::vector<float> position) {
		vec.push_back(0);
		vector_light_ = { vec };
		position.push_back(0);
		position_ = { position };
	}

	float faceLightIntensity(Matrix<float> vector_normal, std::vector<Matrix<float>> face) {
		// intensity_light_source - интенсивность источника света
		// coeff_diffuse_reflection_light_source - коэффициент диффузного отражения источника света,
		//										   зависящий от материала поверхности и длины волны
		// intensity_scattered_light - интенсивность рассеяного света
		// coef_diffuse_reflection_scattered_light - коэффициент отражения рассеянного света
		// distance_to_face - расстояние от грани до источника света 
		// coeff_for_distance - коэффициент расстояния, влияет на затухание интенсивности света с расстоянием
		// coeff_reflection_curve - коэффициент кривой отражения
		// n - от 1 до 200, влияет на размер светового блика, для негладких поверхностей типа мела или сажи n = 1


		float cosine_light_and_normal = cosineVectors(vector_normal, vector_light_);
		if (cosine_light_and_normal < 0) cosine_light_and_normal = 0;

		Matrix<float> face_сenter(1, 4);
		for (auto face_itr : face)
			face_сenter = face_сenter + face_itr;
		face_сenter = face_сenter / (float)face.size();

		float distance_to_face = distanceBetweenVertices(face_сenter, position_);

		//float cosine_reflection_and_observation = cosineVectors();

		float light_intensity = intensity_scattered_light * coef_diffuse_reflection_scattered_light
			+ intensity_light_source / (coeff_for_distance + distance_to_face)
			* (coeff_diffuse_reflection_light_source * cosine_light_and_normal
				/* + coeff_reflection_curve * pow(cosine_reflection_and_observation, n)*/);

		if (light_intensity < 0) light_intensity = 0;
		if (light_intensity > 255) light_intensity = 255;
		return light_intensity / 255;
	}
	/*void showLightSource(Camera camera, HDC hdc) {
		Object3D light;
		light.vertexesAndFaces({ position_,  (position_ + vector_light_) }, { {0, 1} });
		light.colors_faces_ = { 0xff0000, 0x00ff00, 0x0000ff, 0x00ff00, 0x0000ff, 0xff0000 };
		light.line_width_ = 2;
		light.drawFrame(camera, hdc);
	}*/

	~Light() { }
};
#endif LIGHT_H