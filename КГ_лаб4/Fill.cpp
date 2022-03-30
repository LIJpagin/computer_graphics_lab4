#include "Fill.h"

void Fill::fillLines(HDC hdc, std::vector<Matrix<int>> points, COLORREF color) {
	SetBkColor(hdc, color);
	buildEdgeList(points);
	Edge* active = new Edge;
	active->next = nullptr;
	for (int scan = 0; scan < window_height_; scan++) {
		buildActiveList(scan, active);
		if (active->next) {
			Edge* p1 = active->next, * p2;
			while (p1) {
				p2 = p1->next;
				if (p1->xIntersect > 1 && p2->xIntersect > 1) {
					HPEN hPen;
					hPen = CreatePen(PS_DASHDOT, 1, color);
					SelectObject(hdc, hPen);
					MoveToEx(hdc, (int)p1->xIntersect, scan, nullptr);
					LineTo(hdc, (int)p2->xIntersect, scan);
					DeleteObject(hPen);
				}
				p1 = p2->next;
			}
			updateActiveList(scan, active);
			resortActiveList(active);
		}
	}
}


void Fill::buildEdgeList(std::vector<Matrix<int>> points) {
	Edge* edge = nullptr;
	Matrix<int> v1, v2;
	int yPrev = (int)points[points.size() - 2](0, 1);
	v1 = points[points.size() - 1];
	for (int i = 0; i < points.size(); i++) {
		v2 = points[i];
		if (v1(0, 1) != v2(0, 1)) {
			edge = (Edge*)malloc(sizeof(Edge));
			if (v1(0, 1) < v2(0, 1))
				makeEdgeRec(v1, v2, yNext(i, points), edge);
			else
				makeEdgeRec(v2, v1, yPrev, edge);
		}
		yPrev = (int)v1(0, 1);
		v1 = v2;
	}
}

int Fill::yNext(int k, std::vector<Matrix<int>> points) {
	int j;
	if (k + 1 > (int)points.size() - 1) j = 0;
	else j = k + 1;
	while (points[k](0, 1) == points[j](0, 1))
		if (j + 1 > (int)points.size() - 1) j = 0;
		else j++;
	return points[j](0, 1);
}

void Fill::makeEdgeRec(Matrix<int> lower, Matrix<int> upper, int yComp, Edge* edge) {
	edge->dxPerScan = (float)(upper(0, 0) - lower(0, 0)) / (upper(0, 1) - lower(0, 1));
	edge->xIntersect = lower(0, 0);
	if (upper(0, 1) < yComp) edge->yUpper = (int)upper(0, 1) - 1;
	else edge->yUpper = (int)upper(0, 1);
	insertEdge(edges_[(int)lower(0, 1)], edge);
}

void Fill::insertEdge(Edge* list, Edge* edge) {
	Edge* p = nullptr, *q = list;
	p = q->next;
	while (p != nullptr)
		if (edge->xIntersect < p->xIntersect) p = nullptr;
		else q = p, p = p->next;
	edge->next = q->next;
	q->next = edge;
}

void Fill::buildActiveList(int scan, Edge* active) {
	Edge* p = nullptr, * q = nullptr;
	p = edges_[scan]->next;
	while (p) {
		q = p->next;
		insertEdge(active, p);
		p = q;
	}
}

void Fill::updateActiveList(int scan, Edge* active) {
	Edge* q = active, * p = active->next;
	while (p)
		if (scan >= p->yUpper) {
			p = p->next;
			deleteAfter(q);
		}
		else {
			p->xIntersect = p->xIntersect + p->dxPerScan;
			q = p, p = p->next;
		}
}

void Fill::deleteAfter(Edge* q) {
	Edge* p = q->next;
	q->next = p->next;
	if (!p) delete p;
}

void Fill::resortActiveList(Edge* active) {
	Edge* q, * p = active->next;
	active->next = nullptr;
	while (p) {
		q = p->next;
		insertEdge(active, p);
		p = q;
	}
}