#include "Camera.h"
#include "Object3D.h"
#include <chrono>
#include <iostream>

class Scene {
private:
	std::vector <Camera> cameras_;
	std::vector <Object3D> objects_;
	int action_object_ = 0, action_camera_ = 0;

public:
	int WidthWndClass = 1600, HeightWndClass = 800;

	void createObjects() {
		Camera camera1(WidthWndClass, HeightWndClass, { 0, 0, 10 });
		Camera camera2(WidthWndClass, HeightWndClass, { 0, 0, 10 });
		cameras_.push_back(camera1);
		cameras_.push_back(camera2);
		Object3D object1("C:\\Users\\Пользователь\\Desktop\\КГ_лаб4\\cube.obj");
		Object3D object2("C:\\Users\\Пользователь\\Desktop\\КГ_лаб4\\ball.obj");
		//objects_.push_back(object1);
		objects_.push_back(object2);
	}
	void show(HDC hdc) {
		for (size_t i = 0; i < objects_.size(); i++)
			objects_[i].draw(cameras_[action_camera_], hdc);
	}
	void control(WPARAM key) {
		float translate_speed = 0.05f;
		float rotate_speed = 0.5f;
		float scale_speed = 0.01f;
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			if (key == int('E')) objects_[action_object_].rotate(0, 0, rotate_speed);
			if (key == int('Q')) objects_[action_object_].rotate(0, 0, -rotate_speed);
			if (key == int('W')) objects_[action_object_].rotate(rotate_speed, 0, 0);
			if (key == int('S')) objects_[action_object_].rotate(-rotate_speed, 0, 0);
			if (key == int('D')) objects_[action_object_].rotate(0, rotate_speed, 0);
			if (key == int('A')) objects_[action_object_].rotate(0, -rotate_speed, 0);
		}
		else if (GetKeyState(VK_CONTROL) & 0x8000) {
			if (key == int('E')) objects_[action_object_].scale(1, 1, 1 + scale_speed);
			if (key == int('Q')) objects_[action_object_].scale(1, 1, 1 - scale_speed);
			if (key == int('W')) objects_[action_object_].scale(1 + scale_speed, 1, 1);
			if (key == int('S')) objects_[action_object_].scale(1 - scale_speed, 1, 1);
			if (key == int('D')) objects_[action_object_].scale(1, 1 + scale_speed, 1);
			if (key == int('A')) objects_[action_object_].scale(1, 1 - scale_speed, 1);
		}
		else {
			if (key == int('O')) action_object_ == objects_.size() - 1 ? action_object_ = 0 : action_object_++;
			if (key == int('C')) action_camera_ == cameras_.size() - 1 ? action_camera_ = 0 : action_camera_++;

			if (key == int('E')) objects_[action_object_].translate(0, 0, translate_speed);
			if (key == int('Q')) objects_[action_object_].translate(0, 0, -translate_speed);
			if (key == int('W')) objects_[action_object_].translate(translate_speed, 0, 0);
			if (key == int('S')) objects_[action_object_].translate(-translate_speed, 0, 0);
			if (key == int('D')) objects_[action_object_].translate(0, translate_speed, 0);
			if (key == int('A')) objects_[action_object_].translate(0, -translate_speed, 0);
		}
		if (key == 105) cameras_[action_camera_].translate(0, 0, translate_speed);
		if (key == 103) cameras_[action_camera_].translate(0, 0, -translate_speed);
		if (key == 101) cameras_[action_camera_].translate(translate_speed, 0, 0);
		if (key == 104) cameras_[action_camera_].translate(-translate_speed, 0, 0);
		if (key == 100) cameras_[action_camera_].translate(0, translate_speed, 0);
		if (key == 102) cameras_[action_camera_].translate(0, -translate_speed, 0);

		if (key == 39) cameras_[action_camera_].rotate(0, rotate_speed, 0);
		if (key == 37) cameras_[action_camera_].rotate(0, -rotate_speed, 0);
		if (key == 40) cameras_[action_camera_].rotate(rotate_speed, 0, 0);
		if (key == 38) cameras_[action_camera_].rotate(-rotate_speed, 0, 0);
	}
	void movement() {

	}
} scene;


LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps; RECT Rect; HDC hdc, hCmpDC; HBITMAP hBmp;

	switch (messg) {

	case WM_PAINT: {
		GetClientRect(hWnd, &Rect);
		hdc = BeginPaint(hWnd, &ps);

		// Создание теневого контекста для двойной буферизации
		hCmpDC = CreateCompatibleDC(hdc);
		hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left, Rect.bottom - Rect.top);
		SelectObject(hCmpDC, hBmp);

		// Закраска фоновым цветом
		LOGBRUSH br;
		br.lbStyle = BS_SOLID;
		br.lbColor = 0x202020;
		HBRUSH brush;
		brush = CreateBrushIndirect(&br);
		FillRect(hCmpDC, &Rect, brush);
		DeleteObject(brush);

		// Отрисовка
		scene.show(hCmpDC);

		// Копируем изображение из теневого контекста на экран
		SetStretchBltMode(hdc, COLORONCOLOR);
		BitBlt(hdc, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, hCmpDC, 0, 0, SRCCOPY);

		// Удаляем ненужные системные объекты
		DeleteDC(hCmpDC);
		DeleteObject(hBmp);
		hCmpDC = NULL;

		EndPaint(hWnd, &ps);
	} break;

	case WM_ERASEBKGND:
		return 1;
		break;

	case WM_KEYDOWN:
		scene.control(wParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(hWnd, messg, wParam, lParam));
	}
	scene.movement();
	InvalidateRect(hWnd, NULL, TRUE);
	return (0);
}
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS WndClass;

	scene.createObjects();

	// Заполняем структуру класса окна
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_CROSS);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = L"CG_WAPI_Template";

	// Регистрируем класс окна
	if (!RegisterClass(&WndClass)) {
		MessageBox(NULL, L"Не могу зарегистрировать класс окна!", L"Ошибка", MB_OK);
		return 0;
	}

	// Создаем основное окно приложения
	hWnd = CreateWindow(
		L"CG_WAPI_Template", // Имя класса 
		L"Компьютерная графика лабораторная работа №1", // Текст заголовка
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, // Стиль окна 
		-12, 0, // Позиция левого верхнего угла 
		scene.WidthWndClass, scene.HeightWndClass, // Ширина и высота окна 
		(HWND)NULL, // Указатель на родительское окно NULL 
		(HMENU)NULL, // Используется меню класса окна 
		(HINSTANCE)hInstance, // Указатель на текущее приложение
		NULL); // Передается в качестве lParam в событие WM_CREATE


	// Если окно не удалось создать
	if (!hWnd) {
		MessageBox(NULL, L"Не удается создать главное окно!", L"Ошибка", MB_OK);
		return 0;
	}

	// Показываем окно
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Выполняем цикл обработки сообщений до закрытия приложения
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}
	return lpMsg.wParam;
}