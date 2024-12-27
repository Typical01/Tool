#include "pch.h"
#include "WindowHost.h"


int Typical_Tool::WindowsSystem::WindowHost::hMenu = 1000;
HINSTANCE Typical_Tool::WindowsSystem::WindowHost::hIns;
UINT Typical_Tool::WindowsSystem::WindowHost::WM_TASKBARCREATED_WH = RegisterWindowMessage("TaskbarCreated");



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

void Typical_Tool::WindowsSystem::WindowFont::SetWindowFont(HWND hwnd)
{
	if (IsWindow(hwnd)) {
		PostMessage(hwnd, WM_SETFONT, (WPARAM)Font, MAKELPARAM(true, 0));  //设置控件字体
	}
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

void Typical_Tool::WindowsSystem::WindowShell::Shell处理(HMENU 菜单, std::vector<ShellConfig>& Shell配置)
{
	
	lgc("Typical_Tool::WindowsSystem::WindowShell::Shell处理");
	

	for (auto tempShell = Shell配置.begin(); tempShell != Shell配置.end(); tempShell++) {
		//判断类型
		Tstr 操作名 = tempShell->操作名;
		Tstr 菜单按键 = tempShell->菜单按键;

		//区分: 程序启动项/程序菜单项s
		if (菜单按键 == "否") {
			程序启动项.push_back(*tempShell);
			lgc("操作名: " + 操作名);
			lgc("  注册: 程序启动项");
			tempShell->OutConfig(); //输出配置
		}
		else {
			int 菜单项ID = WinHost::GetHMENU();
			//int 菜单项总数 = GetMenuItemCount(菜单);

			程序菜单项.insert(std::make_pair(菜单项ID, *tempShell));
			lgc("操作名: " + 操作名);
			lgc("  注册: 程序菜单项");
			//添加菜单项
			if (AppendMenuW(菜单, MF_STRING, 菜单项ID, StringManage::stow(操作名).c_str())) {
				tempShell->OutConfig(); //输出配置
				lgc("  程序菜单项: 成功");
			}
			else {
				lgc("  程序菜单项: 失败");
			}
		}
	}
}

void Typical_Tool::WindowsSystem::WindowShell::执行程序启动项Shell()
{
	//遍历执行所有: 程序启动项
	if (程序启动项.size() != 0) {
		for (auto tempShell = 程序启动项.begin(); tempShell != 程序启动项.end(); tempShell++) {
			auto 操作名 = tempShell->操作名;
			auto Shell操作 = tempShell->Shell操作;
			auto 文件 = tempShell->文件;
			auto 参数 = tempShell->参数;
			auto 窗口显示 = tempShell->窗口显示;

			ExecuteAnalyze(操作名, Shell操作, 文件, 参数, 窗口显示);
		}
	}
	else {
		lgcr("程序启动项Shell: 没有执行项!", wr);
		lgcr();
	}
}

void Typical_Tool::WindowsSystem::WindowShell::执行程序菜单项Shell(int _菜单选项ID)
{
	//查找并执行对应菜单ID的 ShellConfig
	auto temp = 程序菜单项.find(_菜单选项ID);
	if (temp != 程序菜单项.end()) {
		ShellConfig tempShellConfig = temp->second;

		auto 操作名 = tempShellConfig.操作名;
		auto Shell操作 = tempShellConfig.Shell操作;
		auto 文件 = tempShellConfig.文件;
		auto 参数 = tempShellConfig.参数;
		auto 窗口显示 = tempShellConfig.窗口显示;

		ExecuteAnalyze(操作名, Shell操作, 文件, 参数, 窗口显示);
	}
	else {
		lgcr("程序菜单项Shell: 没有找到菜单选项 " + _菜单选项ID, er);
		lgcr();
	}
}

Typical_Tool::WindowsSystem::ShellMessage Typical_Tool::WindowsSystem::WindowShell::ExecuteAnalyze(Tstr 操作名, Tstr Shell操作, Tstr Shell文件, Tstr Shell参数, Tstr 窗口显示)
{
	if (Shell操作 == "打开文件" || Shell操作 == "open") {
		Shell操作 = "open";
		lgc("ExecuteAnalyze: Shell操作模式(打开文件)", ts);
	}
	else if (Shell操作 == "管理员运行" || Shell操作 == "runas") {
		Shell操作 = "runas";
		lgc("ExecuteAnalyze: Shell操作模式(管理员运行)", ts);
	}
	else if (Shell操作 == "打开文件夹" || Shell操作 == "explore") {
		Shell操作 = "explore";
		lgc("ExecuteAnalyze: Shell操作模式(打开文件夹)", ts);
	}
	else {
		lgcr("ExecuteAnalyze: Shell操作模式错误(打开文件/打开文件夹/管理员运行)", wr);
		lgcr("ExecuteAnalyze: 操作名: " + 操作名, wr);
		return ShellMessage();
	}

	int ShowWindow = 0;
	if (窗口显示 == "是") {
		ShowWindow = 5;
	}
	lgc("ExecuteAnalyze: 窗口显示 " + 窗口显示, wr);

	ShellMessage temp(操作名, (int)ShellExecuteW(NULL, stow(Shell操作).c_str(), stow(Shell文件).c_str(), stow(Shell参数).c_str(), NULL, ShowWindow));
	return temp;
}

void Typical_Tool::WindowsSystem::ShellConfig::OutConfig()
{
	lgc("ShellConfig::OutConfig()", ts);
	lgc("操作名: " + this->操作名);
	lgc("菜单按键: " + this->菜单按键);
	lgc("Shell操作: " + this->Shell操作);
	lgc("文件: " + this->文件);
	lgc("参数: " + this->参数);
	lgc("窗口显示: " + this->窗口显示);
	
}




// WindowHost
bool Typical_Tool::WindowsSystem::WindowHost::添加窗口托管(Tstr windowName, HWND& window, int showWindow)
{
	if (!IsWindow(window)) {
		//创建失败
		lg("创建窗口失败! 窗口名: " + windowName, er);
		return false;
	}
	lgc("创建窗口成功! 窗口名: " + windowName, ts);
	
	
	ShowWindow(window, showWindow);
	UpdateWindow(window);

	WinFont.SetWindowFont(window);
	this->窗口.insert(std::make_pair(windowName, window));
	return true;
}

void Typical_Tool::WindowsSystem::WindowHost::设置字体(HFONT hFont)
{
	WinFont.SetFont(hFont);

	for (auto tempHwnd = this->窗口.begin();
		tempHwnd != this->窗口.end(); tempHwnd++) {
		WinFont.SetWindowFont(tempHwnd->second);
	}
}

std::map<Tstr, HWND>& Typical_Tool::WindowsSystem::WindowHost::Get窗口()
{
    return this->窗口;
}

int Typical_Tool::WindowsSystem::WindowHost::GetHMENU()
{
	WindowHost::hMenu++;
	return WindowHost::hMenu;
}

int Typical_Tool::WindowsSystem::WindowHost::注册窗口类(WNDCLASSW& wndClass)
{
	if (!RegisterClassW(&wndClass))
	{
		lg("窗口类注册失败!\n 窗口类名: " + wtos(wndClass.lpszClassName), er);
		return 0;
	}
	lgc("窗口类注册成功! 窗口类名: " + wtos(wndClass.lpszClassName), ts);
	

	return 1;
}