#pragma once

#ifndef _LOCATION_H
#define _LOCATION_H

#include "pch.h"
//#include "Typical_Tool.h"
#include "Log.h"


namespace Typical_Tool {

	namespace WindowsSystem {

		class  WindowLocationInfo {
#define 窗口位置信息 WindowLocationInfo
		private:
			RECT* Rect工作区;
			RECT* Rect客户区;
		public:

			//主窗口
			int 窗口宽;
			int 窗口高;
			int 窗口X;
			int 窗口Y;

			//客户区
			int 客户区宽;
			int 客户区高;
			int 客户区X;
			int 客户区Y;

		public:
			WindowLocationInfo() {}
			WindowLocationInfo(HWND& hwnd)
			{
				//获取窗口大小
				GetWindowRect(hwnd, Rect工作区);
				窗口宽 = Rect工作区->right;
				窗口高 = Rect工作区->bottom;
				窗口X = Rect工作区->left;
				窗口Y = Rect工作区->top;

				//获取窗口客户区范围
				GetClientRect(hwnd, Rect客户区);
				客户区宽 = Rect客户区->right - Rect客户区->left;
				客户区高 = Rect客户区->bottom - Rect客户区->top;
				客户区X = Rect客户区->left;
				客户区Y = Rect客户区->top;
			}

			void GetInfo(HWND& hwnd);
		};
		using WinLocation = WindowLocationInfo;
		using WL = WindowLocationInfo;
	}
}

#endif