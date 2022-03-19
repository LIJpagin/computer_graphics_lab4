#include "Camera.h"
#include "Object3D.h"
#include <chrono>
#include <iostream>

char szClassName[] = "CG_WAPI_Template";
const int WidthWndClass = 1600, HeightWndClass = 800;
Camera camera(WidthWndClass, HeightWndClass, { 0, 0, 10 });
Object3D object;

void control(WPARAM key) {
	float translate_speed = 0.05f;
	float rotate_speed = 2;
	float scale_speed = 0.01f;
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		if (key == int('E')) object.rotate(0, 0,  rotate_speed);
		if (key == int('Q')) object.rotate(0, 0, -rotate_speed);
		if (key == int('W')) object.rotate( rotate_speed, 0, 0);
		if (key == int('S')) object.rotate(-rotate_speed, 0, 0);
		if (key == int('D')) object.rotate(0,  rotate_speed, 0);
		if (key == int('A')) object.rotate(0, -rotate_speed, 0);
	}
	else if (GetKeyState(VK_CONTROL) & 0x8000) {
		if (key == int('E')) object.scale(1, 1, scale_speed);
		if (key == int('Q')) object.scale(1, 1, -scale_speed);
		if (key == int('W')) object.scale(1 + scale_speed, 1, 1);
		if (key == int('S')) object.scale(1 - scale_speed, 1, 1);
		if (key == int('D')) object.scale(1, scale_speed, 1);
		if (key == int('A')) object.scale(1, 1 - scale_speed, 1);
	}
	else {
		if (key == int('E')) object.translate(0, 0, translate_speed);
		if (key == int('Q')) object.translate(0, 0, -translate_speed);
		if (key == int('W')) object.translate(translate_speed, 0, 0);
		if (key == int('S')) object.translate(-translate_speed, 0, 0);
		if (key == int('D')) object.translate(0, translate_speed, 0);
		if (key == int('A')) object.translate(0, -translate_speed, 0);
	}
	if (key == 105) camera.translate(0, 0, translate_speed);
	if (key == 103) camera.translate(0, 0, -translate_speed);
	if (key == 101) camera.translate(translate_speed, 0, 0);
	if (key == 104) camera.translate(-translate_speed, 0, 0);
	if (key == 100) camera.translate(0, translate_speed, 0);
	if (key == 102) camera.translate(0, -translate_speed, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps; RECT Rect; HDC hdc, hCmpDC; HBITMAP hBmp;

	switch (messg) {

	case WM_PAINT: {
		GetClientRect(hWnd, &Rect);
		hdc = BeginPaint(hWnd, &ps);

		// �������� �������� ��������� ��� ������� �����������
		hCmpDC = CreateCompatibleDC(hdc);
		hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left, Rect.bottom - Rect.top);
		SelectObject(hCmpDC, hBmp);

		// �������� ������� ������
		LOGBRUSH br;
		br.lbStyle = BS_SOLID;
		br.lbColor = 0x282828;
		HBRUSH brush;
		brush = CreateBrushIndirect(&br);
		FillRect(hCmpDC, &Rect, brush);
		DeleteObject(brush);

		// ���������
		object.draw(camera, hCmpDC);

		// �������� ����������� �� �������� ��������� �� �����
		SetStretchBltMode(hdc, COLORONCOLOR);
		BitBlt(hdc, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, hCmpDC, 0, 0, SRCCOPY);

		// ������� �������� ��������� �������
		DeleteDC(hCmpDC);
		DeleteObject(hBmp);
		hCmpDC = NULL;

		EndPaint(hWnd, &ps);
	} break;

	case WM_ERASEBKGND:
		return 1;
		break;

	case WM_KEYDOWN:
		control(wParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(hWnd, messg, wParam, lParam));
	}
	//camera.movement();
	InvalidateRect(hWnd, NULL, TRUE);
	return (0);
}
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS WndClass;

	// ��������� ��������� ������ ����
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_CROSS);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = (LPCWSTR)szClassName;

	// ������������ ����� ����
	if (!RegisterClass(&WndClass)) {
		MessageBox(NULL, L"�� ���� ���������������� ����� ����!", L"������", MB_OK);
		return 0;
	}

	// ������� �������� ���� ����������
	hWnd = CreateWindow(
		(LPCWSTR)szClassName, // ��� ������ 
		L"������������ ������� ������������ ������ �1", // ����� ���������
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, // ����� ���� 
		-12, 0, // ������� ������ �������� ���� 
		WidthWndClass, HeightWndClass, // ������ � ������ ���� 
		(HWND)NULL, // ��������� �� ������������ ���� NULL 
		(HMENU)NULL, // ������������ ���� ������ ���� 
		(HINSTANCE)hInstance, // ��������� �� ������� ����������
		NULL); // ���������� � �������� lParam � ������� WM_CREATE


	// ���� ���� �� ������� �������
	if (!hWnd) {
		MessageBox(NULL, L"�� ������� ������� ������� ����!", L"������", MB_OK);
		return 0;
	}

	// ���������� ����
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// ��������� ���� ��������� ��������� �� �������� ����������
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}
	return lpMsg.wParam;
}