#pragma once

#ifndef _HOTKEY_H
#define _HOTKEY_H

#include "pch.h"
//#include "Typical_Tool.h"
#include "Log.h"
#include "Message.h"


namespace Typical_Tool {

	namespace WindowsSystem {

		class  WindowHotkey {
		private:
			static int HotkeyID;
			std::vector<int> HotKeyID_lib;

			std::vector<Ustr> KeyName_lib;
			std::vector<int> KeyID_lib;
			Ustr 组合按键名 = "";
			int 修饰符 = 0;
			bool 基础按键 = false;

		public:
			//热键注册消息 热键注册(组合按键名, RegisterHotKey(hWnd, GetHotkey(), 修饰符组合, 基础按键));
			WindowHotkey() { }
			
			// WM_KEYDOWN
			void 创建组合按键(HWND hWndEdit, WPARAM wParam);
			// WM_KEYUP
			void 清除组合按键(HWND hWndEdit);
			// WM_HOTKEY: 对应窗口
			void 注册按键(HWND hWnd);

			//热键注册消息 热键注册(组合按键名, RegisterHotKey(hWnd, GetHotkey(), 修饰符组合, 基础按键));
			static int GetHotkey();
		};
		using 窗口热键 = WindowHotkey;
	}
}

#endif