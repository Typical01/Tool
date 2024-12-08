#pragma once

//Key.h 按键
#ifndef _KEY_H
#define _KEY_H
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#include "Typical_Tool.h"

#include "pch.h"
#include "Timers.h"

using namespace std;
using Typical_Tool::Timers;

namespace Typical_Tool {

	namespace WindowsSystem {

		enum KeyTriggerMode //按键触发模式
		{
			ClickOne, //单击一次: 按下并抬起
			Down, //按下: 
			Up, //抬起
		};
		using ktm = KeyTriggerMode;

		//ContinuousClick 连续点击
		class  Key
		{
		private:
			//
		public:
			/* 模拟键盘点击: 单击/按下/抬起
			* 间隔时间
			*/
			static void KeyClick(const BYTE& _bVk, const long long& _intervalTime, const ktm& _keyMode = ktm::ClickOne, void(*_func)() = [] {});

			/* 模拟鼠标点击: 鼠标事件(按键按下/抬起, 移动)
			* 间隔时间
			*/
			static void MouseClick(DWORD _mouseEvent, const long long& _intervalTime, void(*_func)() = [] {});

			//获取键入状态
			static bool GetKeyStateDown(const BYTE& _bVk, void(*_func)() = [] {});
			static bool GetKeyStateUp(const BYTE& _bVk, void(*_func)() = [] {});
			//获取键入状态: 窗口处于前台
			static bool GetKeyStateDown_Foreground(const HWND& _hWnd, const BYTE& _bVk, void(*_func)() = [] {});
			static bool GetKeyStateUp_Foreground(const HWND& _hWnd, const BYTE& _bVk, void(*_func)() = [] {});

			//返回锁定键的状态
			static bool GetKeyStateLock(const BYTE& _bVk, void(*_func)() = [] {});
		};
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif