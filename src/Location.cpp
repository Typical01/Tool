#include "pch.h"
#include "Location.h"


void Typical_Tool::WindowsSystem::WindowLocationInfo::GetInfo(HWND& hwnd)
{
	//获取窗口大小
	GetWindowRect(hwnd, Rect工作区);
	窗口宽 = Rect工作区->right - Rect工作区->left;
	窗口高 = Rect工作区->bottom - Rect工作区->top;
	窗口X = Rect工作区->left;
	窗口Y = Rect工作区->top;
	lgc("窗口宽: " + Tto_string(窗口宽), lm::wr);
	lgc("窗口高: " + Tto_string(窗口高), lm::wr);
	lgc("窗口X: " + Tto_string(窗口X), lm::wr);
	lgc("窗口Y: " + Tto_string(窗口Y), lm::wr);

	//获取窗口客户区范围
	GetClientRect(hwnd, Rect客户区);
	客户区宽 = Rect客户区->right - Rect客户区->left;
	客户区高 = Rect客户区->bottom - Rect客户区->top;
	客户区X = Rect客户区->left;
	客户区Y = Rect客户区->top;
	lgc("客户区宽: " + Tto_string(客户区宽), lm::wr);
	lgc("客户区高: " + Tto_string(客户区高), lm::wr);
	lgc("客户区X: " + Tto_string(客户区X), lm::wr);
	lgc("客户区Y: " + Tto_string(客户区Y), lm::wr);
}
