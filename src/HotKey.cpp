#include "pch.h"
#include "Hotkey.h"


int Typical_Tool::WindowsSystem::WindowHotkey::HotkeyID = 10000;


void Typical_Tool::WindowsSystem::WindowHotkey::创建组合按键(HWND hWndEdit, WPARAM wParam)
{
	// 手动构建按键名称 
	// 修饰符 
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		KeyName_lib.push_back(_T("Shift"));
		KeyID_lib.push_back(MOD_SHIFT);
		修饰符 += 1;
	}
	else if (GetKeyState(VK_CONTROL) & 0x8000) {
		KeyName_lib.push_back(_T("Ctrl"));
		KeyID_lib.push_back(MOD_CONTROL);
		修饰符 += 1;
	}
	else if (GetKeyState(VK_MENU) & 0x8000) {
		KeyName_lib.push_back(_T("Alt"));
		KeyID_lib.push_back(MOD_ALT);
		修饰符 += 1;
	}

	//虚拟键码
	//数字
	if (基础按键) {
		if (wParam >= 'A' && wParam <= 'Z') {
			KeyName_lib.push_back(Uto_string(wParam));
			KeyID_lib.push_back(wParam);
			基础按键 = true;
		}
		else if (wParam >= '0' && wParam <= '9') {
			KeyName_lib.push_back(Uto_string(wParam));
			KeyID_lib.push_back(wParam);
			基础按键 = true;
		}

		//F键
		if (GetKeyState(VK_F1) & 0x8000) {
			KeyName_lib.push_back(_T("F1"));
			KeyID_lib.push_back(VK_F1);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F2) & 0x8000) {
			KeyName_lib.push_back(_T("F2"));
			KeyID_lib.push_back(VK_F2);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F3) & 0x8000) {
			KeyName_lib.push_back(_T("F3"));
			KeyID_lib.push_back(VK_F3);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F4) & 0x8000) {
			KeyName_lib.push_back(_T("F4"));
			KeyID_lib.push_back(VK_F4);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F5) & 0x8000) {
			KeyName_lib.push_back(_T("F5"));
			KeyID_lib.push_back(VK_F5);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F6) & 0x8000) {
			KeyName_lib.push_back(_T("F6"));
			KeyID_lib.push_back(VK_F6);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F7) & 0x8000) {
			KeyName_lib.push_back(_T("F7"));
			KeyID_lib.push_back(VK_F7);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F8) & 0x8000) {
			KeyName_lib.push_back(_T("F8"));
			KeyID_lib.push_back(VK_F8);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F9) & 0x8000) {
			KeyName_lib.push_back(_T("F9"));
			KeyID_lib.push_back(VK_F9);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F10) & 0x8000) {
			KeyName_lib.push_back(_T("F10"));
			KeyID_lib.push_back(VK_F10);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F11) & 0x8000) {
			KeyName_lib.push_back(_T("F11"));
			KeyID_lib.push_back(VK_F11);
			基础按键 = true;
		}
		else if (GetKeyState(VK_F12) & 0x8000) {
			KeyName_lib.push_back(_T("F12"));
			KeyID_lib.push_back(VK_F12);
			基础按键 = true;
		}

		//符号
		if (GetKeyState(VK_OEM_3) & 0x8000) {
			KeyName_lib.push_back(_T("`"));
			KeyID_lib.push_back(VK_OEM_3);
			基础按键 = true;
		}
	}

	// 获取编辑控件的文本
	Uchar buffer[256];
	lgc(_T("Typical_Tool::WindowsSystem::WindowHotkey::创建组合按键名()::GetWindowText()之前用于对比返回值的错误代码: ") + Uto_string(GetLastError()), lgm::er);
	int charLen = GetWindowText(hWndEdit, buffer, sizeof(buffer) / sizeof(buffer[0]));
	if (charLen == 0) {
		lgc(_T("Typical_Tool::WindowsSystem::WindowHotkey::创建组合按键名()::GetWindowText() 错误代码: ") + Uto_string(GetLastError()), lgm::er);
	}
	// 根据修饰符个数判断 存储的位置
	if (修饰符 > 1) {
		组合按键名 = (Ustr)buffer + _T(" + ") + KeyName_lib[修饰符 - 1];
	}
	else {
		组合按键名 = KeyName_lib[0];
	}
	SetWindowText(hWndEdit, 组合按键名.c_str());
}

void Typical_Tool::WindowsSystem::WindowHotkey::清除组合按键(HWND hWndEdit)
{
	基础按键 = false;
	修饰符 = 0;
	KeyName_lib.clear();
	KeyID_lib.clear();
	组合按键名 = _T("");
	SetWindowText(hWndEdit, _T(""));
}

void Typical_Tool::WindowsSystem::WindowHotkey::注册按键(HWND hWnd)
{
	bool 注册条件 = false;
	int 修饰符组合 = 0;
	int 基础按键 = 0;

	//不低于两个的按键
	if ((KeyID_lib.size() - 1) > 1) {
		if (修饰符 > 0) {
			if (基础按键) {
				// 起码是: Ctrl(修饰符) + 1(基础按键) 才行
				注册条件 = true;
			}
		}
	}

	if (注册条件) {
		for (int i = 0; i < KeyID_lib.size(); i++) {
			if (KeyID_lib[i] == MOD_CONTROL) {
				修饰符组合 |= MOD_CONTROL;
			}
			else if (KeyID_lib[i] == MOD_SHIFT) {
				修饰符组合 |= MOD_SHIFT;
			}
			else if (KeyID_lib[i] == MOD_ALT) {
				修饰符组合 |= MOD_ALT;
			}
			else {
				基础按键 |= KeyID_lib[i];
			}
		}
	}

	热键注册消息 热键注册(组合按键名, RegisterHotKey(hWnd, GetHotkey(), 修饰符组合, 基础按键));
}

int Typical_Tool::WindowsSystem::WindowHotkey::GetHotkey()
{
	HotkeyID++;
	return HotkeyID;
}
