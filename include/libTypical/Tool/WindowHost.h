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
		class WindowFont {
		private:
			static HFONT Font;

		public:
			WindowFont() {
				Font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

				Font = CreateFont(
					-16, -7, 0, 0,
					400, //粗度 一般这个值设为400
					FALSE, //斜体
					FALSE, //下划线
					FALSE, //删除线
					DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
					FF_DONTCARE,
					TEXT("微软雅黑")
				);

			}
			WindowFont(HFONT hFont) {
				Font = hFont;
			}

			HFONT GetFont() { return Font; }

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
					lgc(Tip, Printf(TEXT("热键注册[%s]: 成功!"), this->Message));
				}
				else {
					lgc(Err, Printf(TEXT("热键注册[%s]: 错误!\n\t代码(%s)"), this->Message, ToStr(message)));
				}
			}
			RegisterHotKeyMessage(Tstr&& RegisterHotKeyMessage, int message)
				: Message(RegisterHotKeyMessage)
			{
				if (message > 0) {
					lgc(Tip, Printf(TEXT("热键注册[%s]: 成功!"), this->Message));
				}
				else {
					lgc(Err, Printf(TEXT("热键注册[%s]: 错误!\n\t代码(%s)"), this->Message, ToStr(message)));
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
			ShellMessage() : Message(TEXT("")), ErrorCode(0), Status(false) {
				lgc(Err, Printf(TEXT("Shell消息[%s]: 非Shell错误()"), this->Message));
			}
			ShellMessage(const Tstr& ShellMessage, int message)
				: Message(ShellMessage), ErrorCode(message)
			{
				if (ErrorCode < 32) {
					this->Status = false;
					//ShellExecute() 成功操作, 则传入为句柄
					lgc(Err, Printf(TEXT("Shell消息[%s]: 错误!\n\t代码(%s)"), this->Message, ToStr(ErrorCode)));
				}
				else {
					this->Status = true;
					lgc(Tip, Printf(TEXT("Shell消息[%s]: 成功!"), this->Message));
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
			static inline UINT WM_TASKBARCREATED_WH = RegisterWindowMessage(TEXT("TaskbarCreated"));

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
					lg(Err, Printf(TEXT("创建窗口失败!\n\t窗口名: [%s]"), windowName));
					return false;
				}
				lgc(Tip, Printf(TEXT("创建窗口成功!\n\t窗口名: [%s]"), windowName));


				ShowWindow(window, showWindow);
				UpdateWindow(window);

				WinFont.SetWindowFont(window);
				this->Window.insert(std::make_pair(windowName, window));
				return true;
			}

			WindowFont GetWindowFont() { return this->WinFont; }

			void SetFont(HFONT hFont);
		public:

			static int RegisterWindowClass(WNDCLASS& wndClass) {
				if (!RegisterClass(&wndClass)) {
					lg(Err, Printf(TEXT("窗口类注册失败!\n\t窗口类名: [%s]"), wndClass.lpszClassName));
					return 0;
				}
				lgc(Tip, Printf(TEXT("窗口类注册成功!\n\t窗口类名: [%s]"), wndClass.lpszClassName));


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

			ShellConfig(Tstr _OperateName, Tstr _ShellOperate, Tstr _File,
				Tstr _Arg, Tstr _WindowShow = TEXT("是"), Tstr _MenuButton = TEXT("否"))
				: OperateName(_OperateName), ShellOperate(_ShellOperate), File(_File), Arg(_Arg), WindowShow(_WindowShow), MenuButton(_MenuButton)
			{}

			ShellConfig(const ShellConfig & Other)
				: OperateName(Other.OperateName),
				ShellOperate(Other.ShellOperate),
				File(Other.File),
				Arg(Other.Arg),
				WindowShow(Other.WindowShow),
				MenuButton(Other.MenuButton) {
				// 如果有需要深拷贝的成员，在这里处理
			}

			// 复制赋值运算符重载，实现深拷贝
			ShellConfig& operator=(const ShellConfig & Other) {
				if (this != &Other) {
					OperateName = Other.OperateName;
					ShellOperate = Other.ShellOperate;
					File = Other.File;
					Arg = Other.Arg;
					WindowShow = Other.WindowShow;
					MenuButton = Other.MenuButton;
				}
				return *this;
			}

			// 移动赋值运算符重载，实现资源的高效转移
			ShellConfig& operator=(ShellConfig && Other) noexcept {
				if (this != &Other) {
					OperateName = std::move(Other.OperateName);
					ShellOperate = std::move(Other.ShellOperate);
					File = std::move(Other.File);
					Arg = std::move(Other.Arg);
					WindowShow = std::move(Other.WindowShow);
					MenuButton = std::move(Other.MenuButton);
				}
				return *this;
			}

			bool operator<(const ShellConfig& other) const
			{
				if (this->OperateName < other.OperateName) {
					return true;
				}
				else {
					return false;
				}
			}

			void OutConfig()
			{
				lgc(Tip, TEXT("ShellConfig::OutConfig()"));
				lgc(TEXT("菜单按钮: ") + this->MenuButton);
				lgc(TEXT("Shell操作: ") + this->ShellOperate);
				lgc(TEXT("文件: ") + this->File);
				lgc(TEXT("参数: ") + this->Arg);
				lgc(TEXT("显示窗口: ") + this->WindowShow);

			}
		};

		// Shell执行
		class  WindowShell {
		private:
			std::vector<ShellConfig> ExeRunItem; //程序启动项
			std::map<int, ShellConfig> ExeMenuItem; //程序菜单项

		public:
			WindowShell() : ExeRunItem(), ExeMenuItem() { }

			void Clear()
			{
				if (!ExeRunItem.empty()) {
					ExeRunItem.clear();
				}
				if (!ExeMenuItem.empty()) {
					ExeMenuItem.clear();
				}
			}

		public:
			void ShellOperate(HMENU Menu, std::vector<ShellConfig>& _ShellConfig) {
				lgc(TEXT("Typical_Tool::WindowsSystem::WindowShell::ShellOperate"));

				for (auto tempShell = _ShellConfig.begin(); tempShell != _ShellConfig.end(); tempShell++) {
					//判断类型
					Tstr OperateName = tempShell->OperateName;
					Tstr MenuButton = tempShell->MenuButton;

					//区分: 程序启动项/程序菜单项
					if (MenuButton == TEXT("否")) {
						ExeRunItem.push_back(*tempShell);
						lgc(Printf(TEXT("操作名: [%s]"), OperateName));
						lgc(TEXT("  注册: 程序启动项"));
						tempShell->OutConfig(); //输出配置
					}
					else {
						int tempMenuID = WindowHost::GetHMENU();

						ExeMenuItem.insert({ tempMenuID, *tempShell });
						lgc(Printf(TEXT("操作名: [%s]"), OperateName));
						lgc(TEXT("  注册: 程序菜单项"));
						//添加菜单项
						if (AppendMenu(Menu, MF_STRING, tempMenuID, OperateName.c_str())) {
							tempShell->OutConfig(); //输出配置
							lgc(TEXT("  程序菜单项: 成功!"));
						}
						else {
							lgc(TEXT("  程序菜单项: 失败!"));
						}
					}
				}
			}
			//程序启动项Shell
			void ExeRunItemShell() {
				//遍历执行所有: 程序启动项
				if (ExeRunItem.size() > 0) {
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
					lgcr(War, TEXT("ExeRunItemShell: 没有执行项!"));
				}
			}
			//程序菜单项Shell
			void ExeMenuItemShell(int _MenuItemID) {
				//查找并执行对应菜单ID的 Shell配置
				auto tempIndex = ExeMenuItem.find(_MenuItemID);
				if (tempIndex != ExeMenuItem.end()) {
					ShellConfig tempShellConfig = tempIndex->second;

					Tstr OperateName = tempShellConfig.OperateName;
					Tstr ShellOperate = tempShellConfig.ShellOperate;
					Tstr File = tempShellConfig.File;
					Tstr Arg = tempShellConfig.Arg;
					Tstr WindowShow = tempShellConfig.WindowShow;

					ExecuteAnalyze(OperateName, ShellOperate, File, Arg, WindowShow);
				}
				else {
					lgcr(Err, Printf(TEXT("ExeMenuItemShell: 没有找到菜单选项 [%s]!"), _MenuItemID));
				}
			}

			/* WindowShow: 0(SW_SHOW)隐藏窗口
			* ShellOperate: runas / open / explore
			* ShellFile: cmd, note
			* ShellArg: /k
			* I
			* Shell消息 temp("nvidia-smi", (int)ShellExecute(NULL, "runas", "cmd", "nvidia-smi -lgc 1080", NULL, SW_SHOWNORMAL));
			*/
			static ShellMessage ExecuteAnalyze(Tstr OperateName, Tstr ShellOperate, Tstr ShellFile, Tstr ShellArg = TEXT(""), Tstr WindowShow = TEXT("是")) {
				if (ShellOperate == TEXT("打开文件") || ShellOperate == TEXT("open")) {
					ShellOperate = TEXT("open");
					lgc(Tip, TEXT("ExecuteAnalyze: Shell操作模式(打开文件)"));
				}
				else if (ShellOperate == TEXT("管理员运行") || ShellOperate == TEXT("runas")) {
					ShellOperate = TEXT("runas");
					lgc(Tip, TEXT("ExecuteAnalyze: Shell操作模式(管理员运行)"));
				}
				else if (ShellOperate == TEXT("打开文件夹") || ShellOperate == TEXT("explore")) {
					ShellOperate = TEXT("explore");
					lgc(Tip, TEXT("ExecuteAnalyze: Shell操作模式(打开文件夹)"));
				}
				else {
					lgcr(War, TEXT("ExecuteAnalyze: Shell操作模式错误(打开文件/打开文件夹/管理员运行)"));
					lgcr(War, Printf(TEXT("ExecuteAnalyze: 操作名: [%s]"), OperateName));
					return ShellMessage();
				}

				int ShowWindow = 0;
				if (WindowShow == TEXT("是")) {
					ShowWindow = 5;
				}
				lgc(War, Printf(TEXT("ExecuteAnalyze: 窗口显示 [%s]"), WindowShow));

				ShellMessage temp(OperateName, (int)(long long)ShellExecute(NULL, ShellOperate.c_str(), ShellFile.c_str(), ShellArg.c_str(), NULL, ShowWindow));
				return temp;
			}
		};



		//显示--------------------------------------------------------------------------------------------------------------------

		/*设置屏幕分辨率 运行
			* 分辨率: 需要是系统中有的比例, 如: 1920 x 1080(16:9), 1280 x 720(16:9)
		*/
		template<class T = bool>
		void SetDisplaySize(int WidthValue, int HeightValue, const Tstr& DisplayName = L"") {
			DISPLAY_DEVICE displayDevice;
			displayDevice.cb = sizeof(DISPLAY_DEVICE);

			// 枚举所有显示器
			for (int i = 0; EnumDisplayDevices(NULL, i, &displayDevice, 0); i++) {
				// 如果 DisplayName 为空，或者当前显示器名称匹配
				if (DisplayName.empty() || DisplayName == displayDevice.DeviceName) {
					DEVMODE devMode;
					devMode.dmSize = sizeof(DEVMODE);
					devMode.dmDriverExtra = 0;

					// 获取当前显示设置
					if (EnumDisplaySettings(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &devMode)) {
						// 修改分辨率
						devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
						devMode.dmPelsWidth = WidthValue;
						devMode.dmPelsHeight = HeightValue;

						// 测试分辨率是否可用
						LONG result = ChangeDisplaySettingsEx(displayDevice.DeviceName, &devMode, NULL, CDS_TEST, NULL);
						if (result == DISP_CHANGE_SUCCESSFUL) {
							// 应用新的分辨率
							ChangeDisplaySettingsEx(displayDevice.DeviceName, &devMode, NULL, 0, NULL);
							lgc(Tip, Printf(TEXT("分辨率已成功修改为: %ssx%ss"), ToStr(WidthValue), ToStr(HeightValue)));
						}
						else {
							lgc(Err, Printf(TEXT("无法修改分辨率，错误代码: %ss"), ToStr(result)));
						}
					}
					else {
						lgc(Err, TEXT("无法获取当前显示设置!"));
					}

					// 如果指定了显示器名称，找到后直接返回
					if (!DisplayName.empty()) {
						return;
					}
				}
			}

			// 如果指定了显示器名称但未找到
			if (!DisplayName.empty()) {
				lgc(Err, Printf(TEXT("未找到指定的显示器: %ss"), DisplayName));
			}
		}

		// 分辨率信息
		struct Resolution {
			__int32 Width;
			__int32 Height;
		};

		/* 获取所有支持的分辨率
		* _ResolutionIndex: ENUM_CURRENT_SETTINGS(当前分辨率)
		*/
		template<class T = bool>
		std::vector<Resolution> GetSupportedResolutions(__int32 _ResolutionIndex = ENUM_CURRENT_SETTINGS) {
			std::vector<Resolution> resolutions;
			DEVMODE devMode = { 0 };
			devMode.dmSize = sizeof(DEVMODE);

			if (_ResolutionIndex != 0) {
				// 枚举所有显示设置
				for (__int32 i = 0; EnumDisplaySettings(NULL, i, &devMode); i++) {
					Resolution res;
					res.Width = devMode.dmPelsWidth;
					res.Height = devMode.dmPelsHeight;

					// 避免重复添加相同的分辨率
					bool isDuplicate = false;
					for (const auto& existingRes : resolutions) {
						if (existingRes.Width == res.Width && existingRes.Height == res.Height) {
							isDuplicate = true;
							break;
						}
					}

					if (!isDuplicate) {
						resolutions.push_back(res);
					}
				}
			}
			else {
				EnumDisplaySettings(NULL, _ResolutionIndex, &devMode);
				Resolution res;
				res.Width = devMode.dmPelsWidth;
				res.Height = devMode.dmPelsHeight;
				resolutions.push_back(res);
			}

			return resolutions;
		}

		//程序操作----------------------------------------------------------------------------------------------------------------

		//进程DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
		template<class T = bool>
		void WindowDPI() {
			// 设置 DPI 感知
			
			//#include <Shcore.h>
			//链接 Shcore.lib 库
#ifdef WIN32APP
			SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#else
			if (SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) == NULL) { //传入的值无效
				lgc(TEXT("Windows DPI: 传入的值无效."));
				return;
			}

			lgc(TEXT("Windows DPI: DPI感知(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) 设置成功!"));
#endif
		}

		//单一实例窗口程序
		template<class T = bool>
		int AloneRun(Tstr windowClassName, Tstr windowTitleName) {
			//程序启动初始化
			HWND handle = FindWindow(windowClassName.c_str(), windowTitleName.c_str());
			if (handle != NULL) {
				lgr(War, Printf(TEXT("应用程序已在运行: [%s]"), windowTitleName));
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
				ShellMessage UserAdmin(TEXT("申请管理员权限"), (int)ShellExecute(NULL, TEXT("runas"), strApp.c_str(), NULL, NULL, SW_SHOWNORMAL));
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
				Tchar ExePath[MAX_PATH] = TEXT("");
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

		// 添加注册表项以实现开机自启动
		template<class T = bool>
		bool SetSelfStarting(Tstr valueName, Tstr exePath, Tstr CommandLineParam, bool _bAutoStart) {
			LONG result;
			HKEY hKey;

			// 定义注册表路径，指向当前用户的“启动”项
			Tstr regPath = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

			// 打开注册表项
			result = RegOpenKeyEx(HKEY_CURRENT_USER, regPath.c_str(), 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey); //权限: 查询和设置
			if (result != ERROR_SUCCESS) {
				lgc(Err, Printf(TEXT("打开密钥[%s]失败!"), ToStr(result)));
				return false;
			}

			// 路径自动添加双引号
			if (exePath.front() != TEXT('\"') || exePath.back() != TEXT('\"')) {
				exePath = Printf(TEXT("\"%s\""), exePath); // "%s"
			}

			if (!CommandLineParam.empty()) {
				exePath = Printf(TEXT("%s %s"), exePath, CommandLineParam);
			}

			// 查询指定的注册表值
			DWORD dwType = REG_SZ;
			wchar_t existingValue[MAX_PATH];
			DWORD dwSize = sizeof(existingValue);
			result = RegQueryValueEx(hKey, valueName.c_str(), nullptr, &dwType, reinterpret_cast<LPBYTE>(existingValue), &dwSize);

			if (_bAutoStart) { // 如果需要设置为自启动
				// 如果注册表值不存在，或者存在但路径不同，则添加或更新该值
				if (result == ERROR_FILE_NOT_FOUND || (result == ERROR_SUCCESS && exePath != existingValue)) {
					result = RegSetValueEx(hKey, valueName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(exePath.c_str()), (exePath.size() + 1) * sizeof(wchar_t));
					if (result != ERROR_SUCCESS) {
						lgc(Err, Printf(TEXT("设置注册表值[%s]失败!"), ToStr(result)));
						RegCloseKey(hKey);
						return false;
					}
					lgc(Tip, TEXT("已[设置]开机自启动."));
				}
				else if (result != ERROR_SUCCESS) {
					lgc(Err, Printf(TEXT("设置: 查询注册表值[%s]失败!"), ToStr(result)));
					RegCloseKey(hKey);
					return false;
				}
			}
			else { // 如果不需要设置为自启动
				// 如果注册表值存在，则删除该值
				if (result == ERROR_SUCCESS) {
					result = RegDeleteValue(hKey, valueName.c_str());
					if (result != ERROR_SUCCESS) {
						lgc(Err, Printf(TEXT("删除注册表值[%s]失败!"), ToStr(result)));
						RegCloseKey(hKey);
						return false;
					}
					lgc(Tip, TEXT("已[取消]开机自启动."));
				}
				else if (result != ERROR_FILE_NOT_FOUND) {
					lgc(Err, Printf(TEXT("取消: 查询注册表值[%s]失败!"), ToStr(result)));
					RegCloseKey(hKey);
					return false;
				}
			}

			// 关闭注册表项
			RegCloseKey(hKey);
			lgc(Tip, TEXT("注册表修改成功!"));
			return true;
		}

		//文件操作---------------------------------------------------------------------------------------------------------

		//提取程序目录名
		template<class T = bool>
		bool ExtractExeDirectoryName(Tstr& path) {
			size_t lastSepPos = path.find_last_of(TEXT("\\/"));
			if (lastSepPos != Tstr::npos) {
				path = path.substr(0, lastSepPos); // 不包括最后一个路径分隔符
				return true;
			}
			return false; // 如果找不到路径分隔符，则返回空字符串
		}

		//提取程序名
		template<class T = bool>
		bool ExtractExeName(Tstr& path) {
			size_t lastSepPos = path.find_last_of(TEXT("\\/"));
			if (lastSepPos != Tstr::npos) {
				path = path.substr(lastSepPos + 1, path.size()); // 不包括最后一个路径分隔符

				// 去掉 .exe 后缀
				size_t exePos = path.rfind(TEXT(".exe"));
				if (exePos != Tstr::npos) {
					path = path.substr(0, exePos); // 去掉 .exe 后缀
					return true;
				}
				else {
					return false; // 如果找不到路径分隔符，则返回整个路径
				}
			}
			else {
				return false;
			}
		}

		//获取全路径程序名
		template<class T = bool>
		bool GetExePathName(Tstr& _ExeName) {
			Tchar exePath[MAX_PATH];

			//获取当前程序的全路径
			DWORD length = GetModuleFileName(NULL, exePath, MAX_PATH);
			_ExeName = exePath;
			if (length > 0 && length < MAX_PATH) {
				lgc(Printf(TEXT("当前可执行文件的名称: [%s]"), _ExeName));
				return true;
			}
			else {
				lgc(TEXT("无法获取当前可执行文件的路径!"));
				return false;
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