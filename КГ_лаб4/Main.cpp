#include "Object3D.h"
#include "Camera.h"
#include "Light.h"

#include <chrono>
#include <tchar.h>

class Scene {
private:
	bool show_help_ = false;

public:
	std::vector <Camera> cameras;
	std::vector <Object3D> objects;
	std::vector <Light> light_sources;
	Axes axes;

	int action_object = 0, action_camera = 0;
	int WidthWndClass = 1600, HeightWndClass = 900;
	long delta_time = 0, delta_wheel = 0;
	bool left_button_pressed = false;

	void createObjects() {
		Camera camera1(WidthWndClass, HeightWndClass, { 0, 0, 10 });
		cameras.push_back(camera1);
		Object3D object1;
		object1.cube();
		objects.push_back(object1);
	}
	void show(HDC hdc) {
		axes.drawFrame(cameras[action_camera], hdc);
		for (auto object : objects)
			object.drawFrame(cameras[action_camera], hdc);

		SetBkColor(hdc, 0x444444);
		if (show_help_) {
			TextOut(hdc, 10, 10, L"Управление камерами", 19);
			TextOut(hdc, 10, 26, L"клавиши numpad - перемещение", 28);
			TextOut(hdc, 10, 42, L"стрелки - вращение", 18);
			TextOut(hdc, 10, 58, L"C - смена камеры", 16);
			TextOut(hdc, 10, 90, L"Управление объектами", 20);
			TextOut(hdc, 10, 106, L"W,A,S,D,Q,E - перемещение", 25);
			TextOut(hdc, 10, 122, L"SHIFT + W,A,S,D,Q,E - вращение", 30);
			TextOut(hdc, 10, 138, L"CTRL + W,A,S,D,Q,E - масштабирование", 36);
			TextOut(hdc, 10, 154, L"O - смена объекта", 17);
		}
		else {
			TCHAR buffer[20];
			_stprintf_s(buffer, _T("%d ms"), delta_time);
			TextOut(hdc, 10, 10, buffer, (int)_tcslen(buffer));
		}
		if (left_button_pressed) {
			HPEN hPen;
			hPen = CreatePen(PS_DASHDOT, 2, 0xFFFFFF);
			SelectObject(hdc, hPen);
			MoveToEx(hdc, WidthWndClass / 2 - 10, HeightWndClass / 2, NULL);
			LineTo(hdc, WidthWndClass / 2 + 10, HeightWndClass / 2);
			MoveToEx(hdc, WidthWndClass / 2, HeightWndClass / 2 - 10, NULL);
			LineTo(hdc, WidthWndClass / 2, HeightWndClass / 2 + 10);
			DeleteObject(hPen);
		}
	}
	void control(WPARAM key) {
		float translate_speed = 0.05f;
		float rotate_speed = 1.5f;
		float scale_speed = 0.05f;
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			if (key == int('E')) objects[action_object].rotate(0, 0, rotate_speed);
			if (key == int('Q')) objects[action_object].rotate(0, 0, -rotate_speed);
			if (key == int('W')) objects[action_object].rotate(rotate_speed, 0, 0);
			if (key == int('S')) objects[action_object].rotate(-rotate_speed, 0, 0);
			if (key == int('D')) objects[action_object].rotate(0, rotate_speed, 0);
			if (key == int('A')) objects[action_object].rotate(0, -rotate_speed, 0);
		}
		else if (GetKeyState(VK_CONTROL) & 0x8000) {
			if (key == int('E')) objects[action_object].scale(1, 1, 1 + scale_speed);
			if (key == int('Q')) objects[action_object].scale(1, 1, 1 - scale_speed);
			if (key == int('W')) objects[action_object].scale(1 + scale_speed, 1, 1);
			if (key == int('S')) objects[action_object].scale(1 - scale_speed, 1, 1);
			if (key == int('D')) objects[action_object].scale(1, 1 + scale_speed, 1);
			if (key == int('A')) objects[action_object].scale(1, 1 - scale_speed, 1);
		}
		else {
			if (key == int('E')) objects[action_object].translate(0, 0, translate_speed);
			if (key == int('Q')) objects[action_object].translate(0, 0, -translate_speed);
			if (key == int('W')) objects[action_object].translate(translate_speed, 0, 0);
			if (key == int('S')) objects[action_object].translate(-translate_speed, 0, 0);
			if (key == int('D')) objects[action_object].translate(0, translate_speed, 0);
			if (key == int('A')) objects[action_object].translate(0, -translate_speed, 0);
		}
		if (key == 105) cameras[action_camera].translate(0, 0, translate_speed);
		if (key == 103) cameras[action_camera].translate(0, 0, -translate_speed);
		if (key == 101) cameras[action_camera].translate(translate_speed, 0, 0);
		if (key == 104) cameras[action_camera].translate(-translate_speed, 0, 0);
		if (key == 100) cameras[action_camera].translate(0, translate_speed, 0);
		if (key == 102) cameras[action_camera].translate(0, -translate_speed, 0);

		if (key == int('O')) action_object == objects.size() - 1 ? action_object = 0 : action_object++;
		if (key == int('C')) action_camera == cameras.size() - 1 ? action_camera = 0 : action_camera++;
		if (key == 112) show_help_ = !show_help_;
	}
	void movement() {

	}
} scene;


LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps; RECT Rect; HDC hdc, hCmpDC; HBITMAP hBmp;

	switch (messg) {

	case WM_PAINT: {
		auto begin = std::chrono::high_resolution_clock::now();

		GetClientRect(hWnd, &Rect);
		hdc = BeginPaint(hWnd, &ps);

		// Создание теневого контекста для двойной буферизации
		hCmpDC = CreateCompatibleDC(hdc);
		hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left, Rect.bottom - Rect.top);
		SelectObject(hCmpDC, hBmp);

		// Закраска фоновым цветом
		LOGBRUSH br;
		br.lbStyle = BS_SOLID;
		br.lbColor = 0x444444;
		HBRUSH brush;
		brush = CreateBrushIndirect(&br);
		FillRect(hCmpDC, &Rect, brush);
		DeleteObject(brush);

		SetTextColor(hCmpDC, 0xffffff);
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

		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
		scene.delta_time = (long)elapsed.count();
	} break;

	case WM_ERASEBKGND:
		return 1;
		break;

	case WM_LBUTTONDOWN:
		scene.left_button_pressed = !scene.left_button_pressed;
		scene.left_button_pressed ? ShowCursor(false) : ShowCursor(true);
		SetCursorPos(scene.WidthWndClass / 2, scene.HeightWndClass / 2);
		break;
	case WM_MOUSEWHEEL:
		if (!scene.left_button_pressed)
			break;
			scene.delta_wheel += GET_WHEEL_DELTA_WPARAM(wParam);
			for (; scene.delta_wheel > WHEEL_DELTA; scene.delta_wheel -= WHEEL_DELTA)
				scene.cameras[scene.action_camera].translate(-0.5, 0, 0);
			for (; scene.delta_wheel < 0; scene.delta_wheel += WHEEL_DELTA)
				scene.cameras[scene.action_camera].translate(0.5, 0, 0);
		break;
	case WM_MOUSEMOVE:
		if (!scene.left_button_pressed)
			break;
		POINT position;
		GetCursorPos(&position);
		scene.cameras[scene.action_camera].rotate(
			(position.y - scene.HeightWndClass / 2) * 0.02f,
			(position.x - scene.WidthWndClass / 2) * 0.02f, 0);
		SetCursorPos(scene.WidthWndClass / 2, scene.HeightWndClass / 2);
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
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
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
		WS_VISIBLE | WS_POPUP | WS_MAXIMIZE, // Стиль окна 
		CW_USEDEFAULT, CW_USEDEFAULT, // Позиция левого верхнего угла 
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
	return (int)lpMsg.wParam;
}