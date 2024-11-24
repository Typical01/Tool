#include "pch.h"
#include "WindowHosting.h"


int Typical_Tool::WindowsSystem::WindowHosting::hMenu = 1000;
bool Typical_Tool::WindowsSystem::WindowHosting::注册进度条类 = false;
int Typical_Tool::WindowsSystem::WindowHosting::进度条步进 = 1;
const wchar_t* Typical_Tool::WindowsSystem::WindowHosting::进度条类名 = L"进度条";
const wchar_t* Typical_Tool::WindowsSystem::WindowHosting::进度条主窗口类名 = L"进度条主窗口";
int Typical_Tool::WindowsSystem::WindowHosting::进度条进度百分比 = 0;
HWND Typical_Tool::WindowsSystem::WindowHosting::进度条;
HWND Typical_Tool::WindowsSystem::WindowHosting::进度条主窗口;
HINSTANCE Typical_Tool::WindowsSystem::WindowHosting::hIns;
UINT Typical_Tool::WindowsSystem::WindowHosting::WM_TASKBARCREATED_WH = RegisterWindowMessage("TaskbarCreated");


bool Typical_Tool::WindowsSystem::WindowHosting::添加窗口托管(Tstr windowName, HWND& window, int showWindow)
{
	if (!IsWindow(window)) {
		//创建失败
		lg("创建窗口失败! 窗口名: " + windowName, lm::er);
		return false;
	}
	lgc("创建窗口成功! 窗口名: " + windowName, lm::ts);
	lgc();
	
	ShowWindow(window, showWindow);
	UpdateWindow(window);

	WinFont.SetWindowFont(window);
	this->窗口.insert(std::make_pair(windowName, window));
	return true;
}

void Typical_Tool::WindowsSystem::WindowHosting::设置字体(HFONT hFont)
{
	WinFont.SetFont(hFont);

	for (auto tempHwnd = this->窗口.begin();
		tempHwnd != this->窗口.end(); tempHwnd++) {
		WinFont.SetWindowFont(tempHwnd->second);
	}
}

std::map<Tstr, HWND>& Typical_Tool::WindowsSystem::WindowHosting::Get窗口()
{
    return this->窗口;
}

int Typical_Tool::WindowsSystem::WindowHosting::GetHMENU()
{
	WindowHosting::hMenu++;
	return WindowHosting::hMenu;
}

int Typical_Tool::WindowsSystem::WindowHosting::注册窗口类(WNDCLASSW& wndClass)
{
	if (!RegisterClassW(&wndClass))
	{
		lg("窗口类注册失败!\n 窗口类名: " + wtos(wndClass.lpszClassName), lm::er);
		return 0;
	}
	lgc("窗口类注册成功! 窗口类名: " + wtos(wndClass.lpszClassName), lm::ts);
	lgc();
}

LRESULT Typical_Tool::WindowsSystem::WindowProcedureMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
LONG_PTR Typical_Tool::WindowsSystem::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 填充背景
		RECT rect;
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

		// 计算进度条的位置和大小
		int barWidth = (rect.right - rect.left) * WinHost::进度条进度百分比 / 100;
		RECT barRect = { rect.left + 1, rect.top + 1, rect.left + barWidth - 1, rect.bottom - 1 };
		RECT OldbarRect = { rect.left, rect.top, rect.right - rect.left, rect.bottom };

		// 绘制进度条的细线背景
		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(150, 150, 150)); // 创建1像素宽的黑色实线画笔
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen); // 选择画笔到设备上下文
		Rectangle(hdc, OldbarRect.left, OldbarRect.top, OldbarRect.right, OldbarRect.bottom); // 绘制细线边框
		SelectObject(hdc, hOldPen); // 恢复旧画笔
		DeleteObject(hPen); // 释放画笔对象

		// 绘制进度条本身（内部填充）
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 180, 0)); // 创建绿色画刷
		FillRect(hdc, &barRect, hBrush); // 填充进度条内部
		DeleteObject(hBrush); // 释放画刷对象

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		// 销毁窗口时清理资源
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		/*
		* 防止当Explorer.exe 崩溃以后，程序在系统系统托盘中的图标就消失
		*
		* 原理：Explorer.exe 重新载入后会重建系统任务栏。当系统任务栏建立的时候会向系统内所有
		* 注册接收TaskbarCreated 消息的顶级窗口发送一条消息，我们只需要捕捉这个消息，并重建系
		* 统托盘的图标即可。
		*/

		if (uMsg == WinHost::WM_TASKBARCREATED_WH) {
			SendMessage(hWnd, WM_CREATE, wParam, lParam);
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	}
}

void Typical_Tool::WindowsSystem::WindowHosting::注册进度条窗口类()
{
	if (!注册进度条类) {
		hIns = GetModuleHandle(NULL);

		WNDCLASSW wndclassMain = { 0 };
		WNDCLASSW wndclass = { 0 };

		wndclassMain.style = CS_HREDRAW | CS_VREDRAW;
		wndclassMain.lpfnWndProc = WindowProcedureMain;
		wndclassMain.hInstance = hIns;
		wndclassMain.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclassMain.lpszClassName = 进度条主窗口类名;

		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WindowProcedure;
		wndclass.hInstance = hIns;
		wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszClassName = 进度条类名;

		注册窗口类(wndclassMain);
		注册窗口类(wndclass);

		注册进度条类 = true;
	}
}
void Typical_Tool::WindowsSystem::WindowHosting::创建进度条(Tstr 进度条标题栏名)
{
	if (进度条主窗口 == NULL) {
		进度条主窗口 = CreateWindowExW(0,
			进度条主窗口类名, stow(进度条标题栏名).c_str(),
			WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			400,
			80,
			NULL, NULL, hIns, NULL);
		进度条 = CreateWindowExW(0,
			进度条类名, NULL,
			WS_CHILD | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			400,
			40,
			进度条主窗口, NULL, hIns, NULL);
		if (!IsWindow(进度条)) {
			//创建失败
			lg("进度条创建失败! 进度条标题栏名: " + 进度条标题栏名, lm::er);
			return;
		}
		进度条进度百分比 = 0;

		ShowWindow(进度条主窗口, SW_SHOW);
		UpdateWindow(进度条主窗口);
	}
	else {
		ShowWindow(进度条主窗口, true);
	}
}

void Typical_Tool::WindowsSystem::WindowHosting::增加进度条进度(int 进度百分比)
{
	if (IsWindow(进度条主窗口)) {
		进度条进度百分比 = 进度百分比;

		if (进度条进度百分比 >= 100) {
			进度条进度百分比 = 0;
			ShowWindow(进度条主窗口, SW_HIDE);
			//DestroyWindow(进度条主窗口);
			return;
		}
		InvalidateRect(进度条, NULL, TRUE);
		SendMessage(进度条, WM_PAINT, 0, 0);
		lgc("进度条进度百分比: " + Tto_string(进度条进度百分比), lm::ts);
		lgc();
	}
}
void Typical_Tool::WindowsSystem::WindowHosting::增加进度条进度()
{
	if (IsWindow(进度条主窗口)) {
		进度条进度百分比 += 进度条步进;

		if (进度条进度百分比 > 100) {
			进度条进度百分比 = 0;
			ShowWindow(进度条主窗口, SW_HIDE);
			//DestroyWindow(进度条主窗口);
			return;
		}
		InvalidateRect(进度条, NULL, TRUE);
		SendMessage(进度条, WM_PAINT, 0, 0);
		lgc("进度条进度百分比: " + Tto_string(进度条进度百分比), lm::ts);
		lgc();
	}
}
void Typical_Tool::WindowsSystem::WindowHosting::设置进度条步进(int 步进)
{
	进度条步进 = 步进;
}
