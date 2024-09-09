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
	lgc("窗口宽: " + Uto_string(窗口宽), lgm::wr);
	lgc("窗口高: " + Uto_string(窗口高), lgm::wr);
	lgc("窗口X: " + Uto_string(窗口X), lgm::wr);
	lgc("窗口Y: " + Uto_string(窗口Y), lgm::wr);

	//获取窗口客户区范围
	GetClientRect(hwnd, Rect客户区);
	客户区宽 = Rect客户区->right - Rect客户区->left;
	客户区高 = Rect客户区->bottom - Rect客户区->top;
	客户区X = Rect客户区->left;
	客户区Y = Rect客户区->top;
	lgc("客户区宽: " + Uto_string(客户区宽), lgm::wr);
	lgc("客户区高: " + Uto_string(客户区高), lgm::wr);
	lgc("客户区X: " + Uto_string(客户区X), lgm::wr);
	lgc("客户区Y: " + Uto_string(客户区Y), lgm::wr);
}
