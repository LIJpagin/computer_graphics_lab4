#pragma once
#ifndef FILL_H
#define FILL_H
#include <windows.h>
#include <vector>
#include "Matrix.h"

class Fill {
private:
	typedef struct tEdge {
		int yUpper;
		float xIntersect, dxPerScan;
		struct tEdge* next = nullptr;
	} Edge;
	Edge** edges_;
	int window_height_;


protected:
	void buildEdgeList(std::vector<Matrix<int>>);
	int yNext(int, std::vector<Matrix<int>>);
	void makeEdgeRec(Matrix<int>, Matrix<int>, int, Edge*);
	void insertEdge(Edge*, Edge*);
	void buildActiveList(int, Edge*);
	void updateActiveList(int, Edge*);
	void deleteAfter(Edge*);
	void resortActiveList(Edge*);


public:
	Fill(int window_height) : window_height_(window_height) {
		edges_ = new Edge*[window_height];
		for (int i = 0; i < window_height; i++) {
			edges_[i] = (Edge*)malloc(sizeof(Edge));
			edges_[i]->next = nullptr;
		}
	};
	void fillLines(HDC, std::vector<Matrix<int>>, COLORREF);
	void fillPixels(HDC, std::vector<Matrix<int>>, COLORREF);
	~Fill() {
		for (int i = 0; i < window_height_; i++)
			delete edges_[i];
		delete edges_;
	}
};

#endif FILL_H