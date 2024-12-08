#include "pch.h" 

#include "Key.h"

using namespace std;


void Typical_Tool::WindowsSystem::Key::KeyClick(const BYTE& _bVk, const long long& _intervalTime, const ktm& _keyMode, void(*_func)())
{
	switch (_keyMode) {
	case ktm::ClickOne: {
		keybd_event(_bVk, 0, 0, 0);
		Timers::sleep_s(_intervalTime);
		keybd_event(_bVk, 0, KEYEVENTF_KEYUP, 0);
		Timers::sleep_s(_intervalTime);

		break;
	}
	case ktm::Down: {

		break;
	}
	case ktm::Up: {
		break;
	}
	}
}

void Typical_Tool::WindowsSystem::Key::MouseClick(DWORD _bVk, const long long& _intervalTime, void(*_func)())
{
	mouse_event(_bVk, 0, 0, 0, 0);

	_func();
}

bool Typical_Tool::WindowsSystem::Key::GetKeyStateDown(const BYTE& _bVk, void(*_func)())
{
	if (GetAsyncKeyState(_bVk) & 0x8000) {
		_func();
		return true;
	}
	else {
		_func();
		return false;
	}
}
bool Typical_Tool::WindowsSystem::Key::GetKeyStateUp(const BYTE& _bVk, void(*_func)())
{
	if (GetAsyncKeyState(_bVk) & 0x8000) {
		_func();
		return false;
	}
	else {
		_func();
		return true;
	}
}

bool Typical_Tool::WindowsSystem::Key::GetKeyStateDown_Foreground(const HWND& _hWnd, const BYTE& _bVk, void(*_func)())
{
	if (GetKeyState(_bVk) && GetForegroundWindow() == _hWnd) {
		_func();
		return true;
	}
	else {
		_func();
		return false;
	}
}
bool Typical_Tool::WindowsSystem::Key::GetKeyStateUp_Foreground(const HWND& _hWnd, const BYTE& _bVk, void(*_func)())
{
	if (GetKeyState(_bVk) && GetForegroundWindow() == _hWnd) {
		_func();
		return false;
	}
	else {
		_func();
		return true;
	}
}

bool Typical_Tool::WindowsSystem::Key::GetKeyStateLock(const BYTE& _bVk, void(*_func)())
{
	if (GetKeyState(VK_CAPITAL) & 0x8000) { //锁定打开
		_func();
		return true;
	}
	else {
		_func();
		return false;
	}
}
