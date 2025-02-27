#pragma once

#ifdef _WINDOWS

#ifndef _WINDOWSHOSTING_H
#define _WINDOWSHOSTING_H

#include <libTypical/Tool/pch.h>
#include <libTypical/Tool/Log.h>
#include <libTypical/Tool/StringManage.h>



using Typical_Tool::StringManage::stow;
using Typical_Tool::StringManage::wtos;

namespace Typical_Tool {
	//Windows系统操作----------------------------------------------------------------------------------------
	namespace WindowsSystem {

		
		// WindowKey
		enum KeyTriggerMode { //按键触发模式 
			ClickOne, //单击一次: 按下并抬起
			Down, //按下: 
			Up, //抬起
		};
		using ktm = KeyTriggerMode;

		//ContinuousClick 连续点击
		class Key {
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
			WindowFont() {
				Font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

				Font = CreateFontA(
					-16, -7, 0, 0,
					400, //粗度 一般这个值设为400
					FALSE, //斜体
					FALSE, //下划线
					FALSE, //删除线
					DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
					FF_DONTCARE,
					"微软雅黑"
				);

			}
			WindowFont(HFONT hFont) {
				Font = hFont;
			}

			void SetFont(HFONT hFont);

		public:
			void SetWindowFont(HWND hwnd) {
				if (IsWindow(hwnd)) {
					PostMessage(hwnd, WM_SETFONT, (WPARAM)Font, MAKELPARAM(true, 0));  //设置控件字体
				}
			}
		};



		// 注册热键消息
		class  RegisterHotKeyMessage
		{
		private:
			Tstr Message;

		public:
			RegisterHotKeyMessage(Tstr& RegisterHotKeyMessage, int message)
				: Message(RegisterHotKeyMessage)
			{
				if (message > 0) {
					lgc(Tip, Format(Tx("热键注册[%]: 成功!"), this->Message));
				}
				else {
					lgc(Err, Format(Tx("热键注册[%]: 错误!\n\t代码(%)"), this->Message, ToStr(message)));
				}
			}
			RegisterHotKeyMessage(Tstr&& RegisterHotKeyMessage, int message)
				: Message(RegisterHotKeyMessage)
			{
				if (message > 0) {
					lgc(Tip, Format(Tx("热键注册[%]: 成功!"), this->Message));
				}
				else {
					lgc(Err, Format(Tx("热键注册[%]: 错误!\n\t代码(%)"), this->Message, ToStr(message)));
				}
			}
		};

		//Shell消息
		class  ShellMessage {
		private:
			Tstr Message;
			int ErrorCode; //错误代码
			bool Status; //是否成功

		public:
			ShellMessage() : Message(Tx("")), ErrorCode(0), Status(false) {
				lgc(Err, Format(Tx("Shell消息[%]: 非Shell错误()"), this->Message));
			}
			ShellMessage(const Tstr& ShellMessage, int message)
				: Message(ShellMessage), ErrorCode(message)
			{
				if (ErrorCode < 32) {
					this->Status = false;
					//ShellExecute() 成功操作, 则传入为句柄
					lgc(Err, Format(Tx("Shell消息[%]: 错误!\n\t代码(%)"), this->Message, ToStr(ErrorCode)));
				}
				else {
					this->Status = true;
					lgc(Tip, Format(Tx("Shell消息[%]: 成功!"), this->Message));
				}
			}

			//错误代码
			int GetErrorCode();

			//是否成功
			bool IsSucceed();
		};



		// 窗口托管
		class  WindowHost {
		public:
			static inline UINT WM_TASKBARCREATED_WH = RegisterWindowMessage(Tx("TaskbarCreated"));

		private:
			WindowFont WinFont; //窗口字体
			std::map<Tstr, HWND> Window; //窗口

		private:
			static HINSTANCE hIns;
			static int hMenu; //自动分配的菜单ID

		public:
			WindowHost()
				: WinFont() {}

			bool AddWindowHost(Tstr windowName, HWND& window, int showWindow = 5) {
				if (!IsWindow(window)) {
					//创建失败!
					lg(Err, Format(Tx("创建窗口失败!\n\t窗口名: [%]"), windowName));
					return false;
				}
				lgc(Tip, Format(Tx("创建窗口成功!\n\t窗口名: [%]"), windowName));


				ShowWindow(window, showWindow);
				UpdateWindow(window);

				WinFont.SetWindowFont(window);
				this->Window.insert(std::make_pair(windowName, window));
				return true;
			}
			void SetFont(HFONT hFont);
		public:

			static int RegisterWindowClass(WNDCLASS& wndClass) {
				if (!RegisterClass(&wndClass)) {
					lg(Err, Format(Tx("窗口类注册失败!\n\t窗口类名: [%]"), wndClass.lpszClassName));
					return 0;
				}
				lgc(Tip, Format(Tx("窗口类注册成功!\n\t窗口类名: [%]"), wndClass.lpszClassName));


				return 1;
			}

			/* 1.WindowHost::单实例运行 | 可以跟 WindowHost::获取管理员权限
			* 2.创建 WNDCLASS wc 窗口类
			* 3.WindowHost::RegisterWindowClass
			* 4.托管 WindowHost::WindowHost(wc)
			* 5.创建主窗口 & 子窗口后, 将其加入托管 WindowHost::添加主窗口托管/添加子窗口托管();
			* 6.其他初始化
			*/
			static void README() {

			}

		public:
			std::map<Tstr, HWND>& GetWindow() {
				return this->Window;
			}
			static int GetHMENU();

		};



		// Shell配置
		class  ShellConfig {
		public:
			Tstr OperateName; //操作名

			Tstr ShellOperate; //Shell操作
			Tstr File; //文件
			Tstr Arg; //参数
			Tstr WindowShow; //窗口显示
			Tstr MenuButton; //菜单按键

			ShellConfig(Tstr _OperateName, Tstr _ShellOperate, Tstr _File, Tstr _Arg = NULL, Tstr _WindowShow = Tx("是"), Tstr _MenuButton = Tx("否"))
				: OperateName(_OperateName), ShellOperate(_ShellOperate), File(_File), Arg(_Arg), WindowShow(_WindowShow), MenuButton(_MenuButton)
			{}

			bool operator<(const ShellConfig& other) const
			{
				if (OperateName < other.OperateName) {
					return true;
				}
				else {
					return false;
				}
			}

			void OutConfig()
			{
				lgc(Tip, Tx("ShellConfig::OutConfig()"));
				lgc(Tx("操作名: ") + this->OperateName);
				lgc(Tx("菜单按钮: ") + this->MenuButton);
				lgc(Tx("Shell操作: ") + this->ShellOperate);
				lgc(Tx("文件: ") + this->File);
				lgc(Tx("参数: ") + this->Arg);
				lgc(Tx("显示窗口: ") + this->WindowShow);

			}
		};

		// Shell执行
		class  WindowShell {
		private:
			std::vector<ShellConfig> ExeRunItem; //程序启动项
			std::map<int, ShellConfig> ExeMenuItem; //程序菜单项

		public:
			WindowShell() { }

		public:
			void ShellOperate(HMENU Menu, std::vector<ShellConfig>& ShellConfig) {
				lgc(Tx("Typical_Tool::WindowsSystem::WindowShell::ShellOperate"));

				for (auto tempShell = ShellConfig.begin(); tempShell != ShellConfig.end(); tempShell++) {
					//判断类型
					Tstr OperateName = tempShell->OperateName;
					Tstr MenuButton = tempShell->MenuButton;

					//区分: 程序启动项/程序菜单项
					if (MenuButton == Tx("否")) {
						ExeRunItem.push_back(*tempShell);
						lgc(Format(Tx("操作名: [%]"), OperateName));
						lgc(Tx("  注册: 程序启动项"));
						tempShell->OutConfig(); //输出配置
					}
					else {
						int MenuItemID = WindowHost::GetHMENU();
						//int 菜单项总数 = GetMenuItemCount(菜单);

						ExeMenuItem.insert(std::make_pair(MenuItemID, *tempShell));
						lgc(Format(Tx("操作名: [%]"), OperateName));
						lgc(Tx("  注册: 程序菜单项"));
						//添加菜单项
						if (AppendMenu(Menu, MF_STRING, MenuItemID, OperateName.c_str())) {
							tempShell->OutConfig(); //输出配置
							lgc(Tx("  程序菜单项: 成功!"));
						}
						else {
							lgc(Tx("  程序菜单项: 失败!"));
						}
					}
				}
			}
			//程序启动项Shell
			void ExeRunItemShell() {
				//遍历执行所有: 程序启动项
				if (ExeRunItem.size() != 0) {
					for (auto tempShell = ExeRunItem.begin(); tempShell != ExeRunItem.end(); tempShell++) {
						auto OperateName = tempShell->OperateName;
						auto ShellOperate = tempShell->ShellOperate;
						auto File = tempShell->File;
						auto Arg = tempShell->Arg;
						auto WindowShow = tempShell->WindowShow;

						ExecuteAnalyze(OperateName, ShellOperate, File, Arg, WindowShow);
					}
				}
				else {
					lgcr(War, Tx("ExeRunItemShell: 没有执行项!"));
					lgcr();
				}
			}
			//程序菜单项Shell
			void ExeMenuItemShell(int _MenuItemID) {
				//查找并执行对应菜单ID的 Shell配置
				auto temp = ExeMenuItem.find(_MenuItemID);
				if (temp != ExeMenuItem.end()) {
					ShellConfig tempShellConfig = temp->second;

					auto OperateName = tempShellConfig.OperateName;
					auto ShellOperate = tempShellConfig.ShellOperate;
					auto File = tempShellConfig.File;
					auto Arg = tempShellConfig.Arg;
					auto WindowShow = tempShellConfig.WindowShow;

					ExecuteAnalyze(OperateName, ShellOperate, File, Arg, WindowShow);
				}
				else {
					lgcr(Err, Format(Tx("ExeMenuItemShell: 没有找到菜单选项 [%]!"), _MenuItemID));
					lgcr();
				}
			}

			/* WindowShow: 0(SW_SHOW)隐藏窗口
			* ShellOperate: runas / open / explore
			* ShellFile: cmd, note
			* ShellArg: /k
			* I
			* Shell消息 temp("nvidia-smi", (int)ShellExecute(NULL, "runas", "cmd", "nvidia-smi -lgc 1080", NULL, SW_SHOWNORMAL));
			*/
			static ShellMessage ExecuteAnalyze(Tstr OperateName, Tstr ShellOperate, Tstr ShellFile, Tstr ShellArg = Tx(""), Tstr WindowShow = Tx("是")) {
				if (ShellOperate == Tx("打开文件") || ShellOperate == Tx("open")) {
					ShellOperate = Tx("open");
					lgc(Tip, Tx("ExecuteAnalyze: Shell操作模式(打开文件)"));
				}
				else if (ShellOperate == Tx("管理员运行") || ShellOperate == Tx("runas")) {
					ShellOperate = Tx("runas");
					lgc(Tip, Tx("ExecuteAnalyze: Shell操作模式(管理员运行)"));
				}
				else if (ShellOperate == Tx("打开文件夹") || ShellOperate == Tx("explore")) {
					ShellOperate = Tx("explore");
					lgc(Tip, Tx("ExecuteAnalyze: Shell操作模式(打开文件夹)"));
				}
				else {
					lgcr(War, Tx("ExecuteAnalyze: Shell操作模式错误(打开文件/打开文件夹/管理员运行)"));
					lgcr(War, Format(Tx("ExecuteAnalyze: 操作名: [%]"), OperateName));
					return ShellMessage();
				}

				int ShowWindow = 0;
				if (WindowShow == Tx("是")) {
					ShowWindow = 5;
				}
				lgc(War, Format(Tx("ExecuteAnalyze: 窗口显示 [%]"), WindowShow));

				ShellMessage temp(OperateName, (int)(long long)ShellExecute(NULL, ShellOperate.c_str(), ShellFile.c_str(), ShellArg.c_str(), NULL, ShowWindow));
				return temp;
			}

		public:
			void Clear() {
				this->ExeRunItem.clear();
				this->ExeMenuItem.clear();
			}
		};



		//显示--------------------------------------------------------------------------------------------------------------------

		/*设置屏幕分辨率 运行
			* 分辨率: 需要是系统中有的比例, 如: 1920 x 1080(16:9), 1280 x 720(16:9)
		*/
		template<class T = bool>
		void SetDisplaySize(int widthValue, int HeightValue) {
			//初始化
			DEVMODE NewDevMode;
			EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &NewDevMode);

			//记录修改Message
			NewDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
			NewDevMode.dmPelsWidth = widthValue;
			NewDevMode.dmPelsHeight = HeightValue;

			//根据修改Message 修改屏幕分辨率
			ChangeDisplaySettings(&NewDevMode, 0);
		}


		//程序操作----------------------------------------------------------------------------------------------------------------

		//进程DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
		template<class T = bool>
		void WindowDPI() {
			//设置DPI感知级别(可选，仅Windows 10 1703及更高版本）
#if(WINVER >= 0x0605)
			if (SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) == NULL) { //传入的值无效
				lgc(Tx("Windows DPI: 传入的值无效."));
			}
			else {
				lgc(Tx("Windows DPI: DPI感知(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) 设置成功!"));

			}
#else
			lgc(Tx("Windows DPI: 无法设置[WINVER < 0x0605]!"));
#endif
		}

		//单一实例窗口程序
		template<class T = bool>
		int AloneRun(Tstr windowClassName, Tstr windowTitleName) {
			//程序启动初始化
			HWND handle = FindWindow(windowClassName.c_str(), windowTitleName.c_str());
			if (handle != NULL) {
				lgr(War, Format(Tx("应用程序已在运行: [%]"), windowTitleName));
				return 0;
			}
			return 1;
		}

		//是否为管理员
		template<class T = bool>
		bool IsUserAdmin() {
			BOOL retVal = FALSE;
			SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
			PSID AdministratorsGroup;
			BOOL result = AllocateAndInitializeSid(
				&NtAuthority,
				2,
				SECURITY_BUILTIN_DOMAIN_RID,
				DOMAIN_ALIAS_RID_ADMINS,
				0, 0, 0, 0, 0, 0,
				&AdministratorsGroup);

			if (result) {
				if (!CheckTokenMembership(NULL, AdministratorsGroup, &retVal)) {
					retVal = FALSE;
				}
				FreeSid(AdministratorsGroup);
			}
			return retVal;
		}

		//获得管理员权限
		template<class T = bool>
		bool GainAdminPrivileges(Tstr strApp) {
			if (!IsUserAdmin()) { //非管理员权限, 则申请
				ShellMessage UserAdmin(Tx("申请管理员权限"), (int)ShellExecute(NULL, Tx("runas"), strApp.c_str(), NULL, NULL, SW_SHOWNORMAL));
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
		bool GetAdmin(bool isGet = true) {
			if (isGet) {
				//获取当前程序的全路径
				Tchar ExePath[MAX_PATH] = Tx("");
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
		bool SetSelfStarting(Tstr valueName, Tstr exePath) {
			LONG result;
			HKEY hKey;

			Tstr regPath = Tx("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

			// 打开注册表项  
			result = RegOpenKeyEx(HKEY_CURRENT_USER, regPath.c_str(), 0, KEY_SET_VALUE, &hKey);
			if (result != ERROR_SUCCESS) {
				lgc(Err, Format(Tx("打开密钥失败!: [%]"), ToStr(result)));
				return false;
			}

			// 设置注册表值  
			result = RegSetValueEx(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE*)exePath.c_str(), ((int)exePath.size() + 1) * sizeof(wchar_t));
			if (result != ERROR_SUCCESS) {
				lgc(Err, Format(Tx("设置注册表值失败!: [%]"), ToStr(result)));
				RegCloseKey(hKey);
				return false;
			}

			RegCloseKey(hKey);
			lgc(Tip, Tx("注册表注册成功!"));
			return true;
		}

		//文件操作---------------------------------------------------------------------------------------------------------

		//提取程序名
		template<class T = bool>
		bool ExtractExeName(Tstr& path) {
			// 去掉 .exe 后缀
			size_t exePos = path.find_last_of(Tx(".exe"));
			if (exePos != Tstr::npos && exePos == path.length() - 4) {
				path = path.substr(0, exePos); // 去掉 .exe 后缀
				return true;
			}

			return false; // 如果找不到路径分隔符，则返回整个路径
		}

		//提取程序目录名
		template<class T = bool>
		bool ExtractExeDirectoryName(Tstr& path) {
			size_t lastSepPos = path.find_last_of(Tx("\\/"));
			if (lastSepPos != Tstr::npos) {
				path = path.substr(0, lastSepPos); // 不包括最后一个路径分隔符
				return true;
			}
			return false; // 如果找不到路径分隔符，则返回空字符串
		}

		//获取程序名
		template<class T = bool>
		bool GetExePathName(Tstr& _ExeName) {
			Tchar exePath[MAX_PATH];

			//获取当前程序的全路径
			DWORD length = GetModuleFileName(NULL, exePath, MAX_PATH);
			_ExeName = exePath;
			if (length > 0 && length < MAX_PATH) {
				if (ExtractExeName(_ExeName)) {
					lgc(Format(Tx("当前可执行文件的名称: [%]"), _ExeName));
				}
				return true;
			}
			else {
				lgc(Tx("无法获取当前可执行文件的路径!"));
				return false;
			}
		}

		//获取程序父目录名
		template<class T = bool>
		bool GetExeParentDirectoryPathName(Tstr& _DirectoryName) {
			Tchar exePath[MAX_PATH];

			//获取当前程序的全路径
			DWORD length = GetModuleFileName(NULL, exePath, MAX_PATH);
			_DirectoryName = exePath;
			if (length > 0 && length < MAX_PATH) {
				if (ExtractExeDirectoryName(_DirectoryName)) {
					lgc(Format(Tx("当前程序目录路径名: [%]"), _DirectoryName));
				}
				return true;
			}
			else {
				lgc(Tx("无法获取当前可执行文件的路径!"));
				return false;
			}
		}

		template<class T = bool>
		bool CreateFolder(const Tstr& folderPath) {
			DWORD attributes = GetFileAttributes(folderPath.c_str());

			// 检查路径是否存在且不是目录  
			if (attributes == INVALID_FILE_ATTRIBUTES) {
				// 路径不存在或出错，尝试创建目录  
				if (CreateDirectory(folderPath.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
					lgc(Tip, Format(Tx("文件夹: [%] 创建成功!"), folderPath));

					return true;
				}
				lgc(Err, Format(Tx("文件夹: [%] 创建失败!!"), folderPath));

				// 创建失败!且不是因为路径已存在  
				return false;
			}
			else if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
				lgc(Tip, Format(Tx("文件夹: [%] 已存在!"), folderPath));
				// 路径已经是一个目录  
				return true;
			}
			lgc(Err, Format(Tx("文件夹: [%] 创建失败!(路径存在, 但不是目录)!"), folderPath));
			// 路径存在但不是目录（可能是一个文件）  
			return false;
		}

		//打开文件夹
		template<class T = bool>
		void OpenFolder(const Tstr& path) {
			ShellMessage OpenFolder(Tx("打开文件夹"), (int)ShellExecute(NULL, NULL, path.c_str(), NULL, NULL, SW_SHOWNORMAL));
			if (!OpenFolder.IsSucceed()) {
				lg(Err, Tx("ShellExecute: 打开文件夹 失败!!"));
			}
		}

		//控制台----------------------------------------------------------------------------------------------------------

		//移动光标到目标位置
		void MoveCursorLocation(int x, int y);
	}
	namespace Win = WindowsSystem;
}

#endif

#endif