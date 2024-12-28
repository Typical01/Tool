#pragma once

#ifndef _WINDOWSHOSTING_H
#define _WINDOWSHOSTING_H

#include "pch.h"
#include "Log.h"
#include "StringManage.h"

using Typical_Tool::StringManage::stow;
using Typical_Tool::StringManage::wtos;

namespace Typical_Tool {
	//Windows系统操作----------------------------------------------------------------------------------------
	namespace WindowsSystem {

			//显示--------------------------------------------------------------------------------------------------------------------

			/*设置屏幕分辨率 运行
				* 分辨率: 需要是系统中有的比例, 如: 1920 x 1080(16:9), 1280 x 720(16:9)
			*/
			void SetDisplaySize(int widthValue, int HeightValue);


			//程序操作----------------------------------------------------------------------------------------------------------------

			//进程DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
			template<class T = bool>
			void WindowDPI()
			{
				//设置DPI感知级别(可选，仅Windows 10 1703及更高版本）
				if (SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) == NULL) { //传入的值无效
					lgc(_T("Windows DPI: 传入的值无效\n"));
				}
				else {
					lgc(_T("Windows DPI: DPI感知(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) 设置成功!\n"));

				}
			}

			//单一实例窗口程序
			template<class T = bool>
			int AloneRun(Tstr windowClassName, Tstr windowTitleName)
			{
				//程序启动初始化
				HWND handle = FindWindow(windowClassName.c_str(), windowTitleName.c_str());
				if (handle != NULL)
				{
					lgr((Tstr)"应用程序已在运行" + windowTitleName + _T("\n"), wr);
					return 0;
				}
				return 1;
			}

			//是否为管理员
			bool IsUserAdmin();

			//获得管理员权限
			template<class T = bool>
			bool GainAdminPrivileges(Tstr strApp)
			{
				if (!IsUserAdmin()) { //非管理员权限, 则申请
					ShellMessage UserAdmin(_T("申请管理员权限"), (int)ShellExecuteW(NULL, _T("runas"), strApp.c_str(), NULL, NULL, SW_SHOWNORMAL));
					//成功申请时, 退出当前进程
					if (UserAdmin.IsSucceed()) {
						return true;
					}
				}

				return false;
			}

			/* 成功获取到管理员权限后, 返回 true
			* 同时, 可以将发出申请的程序退出, 使拥有管理员权限的程序单例运行
			* code:
			*	if (Typical_Tool::WindowsSystem::WindowHost::GetAdmin(true)) { return 0; }
			*/
			template<class T = bool>
			bool GetAdmin(bool isGet = true)
			{
				if (isGet) {
					//获取当前程序的全路径
					Tchar ExePath[MAX_PATH] = _T("");
					GetModuleFileName(NULL, ExePath, MAX_PATH);
					//获得管理员权限
					if (GainAdminPrivileges(ExePath)) {
						return true;
					}
					else {
						return false;
					}
				}
				else {
					return false;
				}
			}

			//添加注册表项以实现 开机自启动
			template<class T = bool>
			bool SetSelfStarting(Tstr valueName, Tstr exePath)
			{
				LONG result;
				HKEY hKey;

				wstring regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

				// 打开注册表项  
				result = RegOpenKeyExW(HKEY_CURRENT_USER, regPath.c_str(), 0, KEY_SET_VALUE, &hKey);
				if (result != ERROR_SUCCESS) {
					lgc(_T("打开密钥失败: ") + To_string(result), er);
					return false;
				}

				// 设置注册表值  
				result = RegSetValueExW(hKey, stow(valueName).c_str(), 0, REG_SZ, (const BYTE*)stow(exePath).c_str(), (stow(exePath).size() + 1) * sizeof(wchar_t));
				if (result != ERROR_SUCCESS) {
					lgc(_T("设置注册表值失败: ") + To_string(result), er);
					RegCloseKey(hKey);
					return false;
				}

				RegCloseKey(hKey);
				lgc(_T("注册表注册成功!"), ts);
				return true;
			}

			//文件操作---------------------------------------------------------------------------------------------------------

			//提取程序名
			template<class T = bool>
			bool ExtractExeName(Tstr& path)
			{
				// 去掉 .exe 后缀
				size_t exePos = path.find_last_of(_T(".exe"));
				if (exePos != Tstr::npos && exePos == path.length() - 4) {
					path = path.substr(0, exePos); // 去掉 .exe 后缀
					return true;
				}

				return false; // 如果找不到路径分隔符，则返回整个路径
			}

			//提取程序目录名
			template<class T = bool>
			bool ExtractExeDirectoryName(Tstr& path)
			{
				size_t lastSepPos = path.find_last_of(_T("\\/"));
				if (lastSepPos != std::wstring::npos) {
					path = path.substr(0, lastSepPos); // 不包括最后一个路径分隔符
					return true;
				}
				return false; // 如果找不到路径分隔符，则返回空字符串
			}

			//获取程序名
			template<class T = bool>
			bool GetExePathName(Tstr& _ExeName)
			{
				Tchar exePath[MAX_PATH];

				//获取当前程序的全路径
				DWORD length = GetModuleFileName(NULL, exePath, MAX_PATH);
				_ExeName = exePath;
				if (length > 0 && length < MAX_PATH) {
					if (ExtractExeName(_ExeName)) {
						lgc(_T("当前可执行文件的名称: ") + _ExeName);
					}
					return true;
				}
				else {
					lgc(_T("无法获取当前可执行文件的路径!"));
					return false;
				}
			}

			//获取程序父目录名
			template<class T = bool>
			bool GetExeParentDirectoryPathName(Tstr& _DirectoryName)
			{
				Tchar exePath[MAX_PATH];

				//获取当前程序的全路径
				DWORD length = GetModuleFileName(NULL, exePath, MAX_PATH);
				_DirectoryName = exePath;
				if (length > 0 && length < MAX_PATH) {
					if (ExtractExeDirectoryName(_DirectoryName)) {
						lgc(_T("当前程序目录路径名: ") + _DirectoryName);
					}
					return true;
				}
				else {
					lgc(_T("无法获取当前可执行文件的路径!"));
					return false;
				}
			}

			template<class T = bool>
			bool CreateFolder(const Tstr& folderPath)
			{
				DWORD attributes = GetFileAttributes(folderPath.c_str());

				// 检查路径是否存在且不是目录  
				if (attributes == INVALID_FILE_ATTRIBUTES)
				{
					// 路径不存在或出错，尝试创建目录  
					if (CreateDirectory(folderPath.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
					{
						lgc(_T("文件夹: ") + folderPath + _T(" 创建成功!"), ts);

						return true;
					}
					lgc(_T("文件夹: ") + folderPath + _T(" 创建失败!"), ts);

					// 创建失败且不是因为路径已存在  
					return false;
				}
				else if (attributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					lgc(_T("文件夹: ") + folderPath + _T(" 已存在"), ts);
					// 路径已经是一个目录  
					return true;
				}
				lgc(_T("文件夹: ") + folderPath + _T(" 创建失败(路径存在, 但不是目录)!"), ts);
				// 路径存在但不是目录（可能是一个文件）  
				return false;
			}

			//打开文件夹
			template<class T = bool>
			void OpenFolder(const Tstr& path)
			{
				ShellMessage OpenFolder(_T("打开文件夹"), (int)ShellExecute(NULL, NULL, path.c_str(), NULL, NULL, SW_SHOWNORMAL));
				if (!OpenFolder.IsSucceed()) {
					lg(_T("ShellExecute: 打开文件夹 失败!"), er);
				}
			}

			//控制台----------------------------------------------------------------------------------------------------------

			//移动光标到目标位置
			void MoveCursorLocation(int x, int y);

		// WindowKey
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

			static void wait_s(long long ms);

			/* 模拟键盘点击: 单击/按下/抬起
			* 间隔时间
			*/
			static void KeyClick(const BYTE& _bVk, const long long& _intervalTime = 10L, const ktm& _keyMode = ktm::ClickOne, void(*_func)() = [] {});

			/* 模拟鼠标点击: 鼠标事件(按键按下/抬起, 移动)
			* 间隔时间
			*/
			static void MouseClick(DWORD _mouseEvent, const long long& _intervalTime = 10L, void(*_func)() = [] {});

			//获取键入状态
			static bool GetKeyState(const BYTE& _bVk, void(*_func)() = [] {});
			//获取键入状态: 窗口处于前台
			static bool GetKeyState_Foreground(const HWND& _hWnd, const BYTE& _bVk, void(*_func)() = [] {});

			//返回锁定键的状态
			static bool GetKeyStateLock(const BYTE& _bVk, void(*_func)() = [] {});
		};



		// WindowFont
		class  WindowFont {
		private:
			static HFONT Font;

		public:
			WindowFont()
			{
				Font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
				
				Font = CreateFont(
					-16, -7, 0, 0,
					400, //粗度 一般这个值设为400
					FALSE, //斜体
					FALSE, //下划线
					FALSE, //删除线
					DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
					FF_DONTCARE,
					_T("微软雅黑")
				);
				
			}
			WindowFont(HFONT hFont) {
				Font = hFont;
			}

			void SetFont(HFONT hFont);

		public:
			void SetWindowFont(HWND hwnd);
		};



		// WindowMessage
		class  RegisterHotKeyMessage
		{
		private:
			Tstr 信息;

		public:
			RegisterHotKeyMessage(Tstr& RegisterHotKey热键信息, int message)
				: 信息(RegisterHotKey热键信息)
			{
				if (message > 0)
				{
					lgc(_T("热键注册[") + this->信息 + _T("]: 成功👌"));
					
				}
				else
				{
					lgc(_T("热键注册[") + this->信息 + _T("]: 错误😒 -> 代码(") + To_string(message) + _T(")"), er);
					
				}
			}
			RegisterHotKeyMessage(Tstr&& RegisterHotKey热键信息, int message)
				: 信息(RegisterHotKey热键信息)
			{
				if (message > 0)
				{
					lgc(_T("热键注册[") + this->信息 + _T("]: 成功👌"));
					
				}
				else
				{
					lgc(_T("热键注册[") + this->信息 + _T("]: 错误😒 -> 代码(") + To_string(message) + _T(")"), er);
					
				}
			}
		};
		typedef RegisterHotKeyMessage 热键注册消息;

		class  ShellMessage
		{
		private:
			Tstr 信息;
			int ErrorCode; //错误代码
			bool Status; //是否成功

		public:
			ShellMessage() {}
			ShellMessage(Tstr& Shell信息, int message)
				: 信息(Shell信息), ErrorCode(0), Status(false)
			{
				if (message < 32)
				{
					this->Status = false;

					//ShellExecute() 成功操作, 则传入为句柄
					this->ErrorCode = message;
					lgc(_T("Shell消息[") + this->信息 + _T("]: 错误😒 -> 代码(") + To_string(message) + _T(")"), er);
					
				}
				else
				{
					this->Status = true;
					this->ErrorCode = message;
					lgc(_T("Shell消息[") + this->信息 + _T("]: 成功 👌"));
					
				}
			}
			ShellMessage(Tstr&& Shell信息, int message)
				: 信息(Shell信息), ErrorCode(0), Status(false)
			{
				if (message < 32)
				{
					this->Status = false;

					//ShellExecute() 成功操作, 则传入为句柄
					this->ErrorCode = message;
					lgc(_T("Shell消息[") + this->信息 + _T("]: 错误😒 -> 代码(") + To_string(message) + _T(")"), er);
					
				}
				else
				{
					this->Status = true;
					this->ErrorCode = message;
					lgc(_T("Shell消息[") + this->信息 + _T("]: 成功 👌"));
					
				}
			}

			//错误代码
			int GetErrorCode();

			//是否成功
			bool IsSucceed();
		};



		// WindowShell
		class  ShellConfig {
		public:
			Tstr 操作名;

			Tstr Shell操作;
			Tstr 文件;
			Tstr 参数;
			Tstr 窗口显示;
			Tstr 菜单按键;

			ShellConfig(Tstr _操作名, Tstr _Shell操作, Tstr _文件, Tstr _参数 = NULL, Tstr _窗口显示 = _T("是"), Tstr _菜单按键 = _T("否"))
				: 操作名(_操作名), Shell操作(_Shell操作), 文件(_文件), 参数(_参数), 窗口显示(_窗口显示), 菜单按键(_菜单按键)
			{}

			bool operator<(const ShellConfig& other) const
			{
				if (操作名 < other.操作名) {
					return true;
				}
				else {
					return false;
				}
			}

			void OutConfig();
		};

		class  WindowShell {
		private:
			std::vector<ShellConfig> 程序启动项;
			std::map<int, ShellConfig> 程序菜单项;

		public:
			WindowShell() {

			}

		public:
			void Shell处理(HMENU 菜单, std::vector<ShellConfig>& Shell配置);
			void 执行程序启动项Shell();
			void 执行程序菜单项Shell(int _菜单选项ID);

			/* 窗口显示: 0(SW_SHOW)隐藏窗口
			* Shell操作: runas / open / explore
			* Shell文件: cmd, note
			* Shell参数: /k
			* I
			* Shell消息 temp("nvidia-smi", (int)ShellExecute(NULL, "runas", "cmd", "nvidia-smi -lgc 1080", NULL, SW_SHOWNORMAL));
			*/
			static ShellMessage ExecuteAnalyze(Tstr 操作名, Tstr Shell操作, Tstr Shell文件, Tstr Shell参数 = _T(""), Tstr 窗口显示 = _T("是"));
		};
		using WinShell = WindowShell;



		// WindowHost
		class  WindowHost {
		public:
			static UINT WM_TASKBARCREATED_WH;

		private:
			WindowFont WinFont;
			std::map<Tstr, HWND> 窗口;

		private:
			static HINSTANCE hIns;
			static int hMenu;

		public:
			WindowHost()
				: WinFont()
			{}

			bool 添加窗口托管(Tstr windowName, HWND& window, int showWindow = 5);
			void 设置字体(HFONT hFont);
		public:
			
			static int 注册窗口类(WNDCLASSW& wndClass);

			/* 1.WindowHost::单实例运行 | 可以跟 WindowHost::获取管理员权限
			* 2.创建 WNDCLASS wc 窗口类
			* 3.WindowHost::注册窗口类
			* 4.托管 WindowHost::WindowHost(wc)
			* 5.创建主窗口 & 子窗口后, 将其加入托管 WindowHost::添加主窗口托管/添加子窗口托管();
			* 6.其他初始化
			*/
			
			static void README()
			{

			}

		public:
			std::map<Tstr, HWND>& Get窗口();
			static int GetHMENU();

		};
		typedef WindowHost WinHost;
	}
	namespace WinSys = WindowsSystem;

}

#endif