#ifdef _WINDOWS

#include <libTypical/Tool/pch.h>
#include <libTypical/Tool/WindowHost.h>


int Typical_Tool::WindowsSystem::WindowHost::hMenu = 1000;
HINSTANCE Typical_Tool::WindowsSystem::WindowHost::hIns;
//UINT Typical_Tool::WindowsSystem::WindowHost::WM_TASKBARCREATED_WH;



void Typical_Tool::WindowsSystem::MoveCursorLocation(int x, int y) {
	COORD pos = { (SHORT)x,(SHORT)y };
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, pos);
}

// WindowKey
void Typical_Tool::WindowsSystem::Key::wait_s(long long ms)
{
	long long timeTarget = (std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()) + std::chrono::milliseconds(ms)).count();
	while (timeTarget > std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count()) {
	}
}

void Typical_Tool::WindowsSystem::Key::KeyClick(const BYTE& _bVk, const long long& _intervalTime, const ktm& _keyMode, void(*_func)())
{
	switch (_keyMode) {
	case ktm::ClickOne: {
		keybd_event(_bVk, 0, 0, 0);
		wait_s(_intervalTime);
		keybd_event(_bVk, 0, KEYEVENTF_KEYUP, 0);
		wait_s(_intervalTime);

		_func();
		break;
	}
	case ktm::Down: {
		keybd_event(_bVk, 0, 0, 0);
		wait_s(_intervalTime);

		_func();
		break;
	}
	case ktm::Up: {
		keybd_event(_bVk, 0, KEYEVENTF_KEYUP, 0);
		wait_s(_intervalTime);

		_func();
		break;
	}
	}
}

void Typical_Tool::WindowsSystem::Key::MouseClick(DWORD _bVk, const long long& _intervalTime, void(*_func)())
{
	mouse_event(_bVk, 0, 0, 0, 0);
	wait_s(_intervalTime);

	_func();
}

bool Typical_Tool::WindowsSystem::Key::GetKeyState(const BYTE& _bVk, void(*_func)())
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

bool Typical_Tool::WindowsSystem::Key::GetKeyState_Foreground(const HWND& _hWnd, const BYTE& _bVk, void(*_func)())
{
	if (GetAsyncKeyState(_bVk) && GetForegroundWindow() == _hWnd) {
		_func();
		return true;
	}
	else {
		_func();
		return false;
	}
}

bool Typical_Tool::WindowsSystem::Key::GetKeyStateLock(const BYTE& _bVk, void(*_func)())
{
	if (::GetKeyState(_bVk) != 0) { //锁定打开
		_func();
		return true;
	}
	else {
		_func();
		return false;
	}
}




// WindowFont
HFONT Typical_Tool::WindowsSystem::WindowFont::Font = NULL;

void Typical_Tool::WindowsSystem::WindowFont::SetFont(HFONT hFont)
{
	this->Font = hFont;
}



// WindowMessage
int Typical_Tool::WindowsSystem::ShellMessage::GetErrorCode()
{
	return this->ErrorCode;
}

bool Typical_Tool::WindowsSystem::ShellMessage::IsSucceed()
{
	return this->Status;
}



// WindowShell



// WindowHost

void Typical_Tool::WindowsSystem::WindowHost::SetFont(HFONT hFont)
{
	WinFont.SetFont(hFont);

	for (auto tempHwnd = this->Window.begin();
		tempHwnd != this->Window.end(); tempHwnd++) {
		WinFont.SetWindowFont(tempHwnd->second);
	}
}

int Typical_Tool::WindowsSystem::WindowHost::GetHMENU()
{
	WindowHost::hMenu++;
	return WindowHost::hMenu;
}






#endif