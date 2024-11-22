#pragma once

//常用工具
#ifndef _COMMONTOOLS_H
#define _COMMONTOOLS_H


#include "pch.h"
//#include "Typical_Tool.h"
#include "Log.h"
#include "Timers.h"
#include "Shell.h"

using namespace std;

namespace Typical_Tool {

	//Windows系统操作----------------------------------------------------------------------------------------
	namespace WindowsSystem
	{

		//显示--------------------------------------------------------------------------------------------------------------------

		/*设置屏幕分辨率 运行
			* **必看**
			* 分辨率(width/height): 一定成比例, 如: 1920 x 1080(16:9), 1280 x 1024(4:3)
		*/
		 void SetDisplaySize(int widthValue, int HeightValue);


		//程序操作----------------------------------------------------------------------------------------------------------------


		//进程DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
		template<class T = bool>
		void WindowDPI()
		{
			//设置DPI感知级别(可选，仅Windows 10 1703及更高版本）
			if (SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) == NULL) { //传入的值无效
				lgc("传入的值无效\n", "Windows DPI");
			}
			else {
				lgc("DPI感知(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) 设置成功!\n", "Windows DPI");
				lgc();
			}
		}

		template<class T = bool>
		int 单实例运行(Tstr windowClassName, Tstr windowTitleName)
		{
			//程序启动初始化
			HWND handle = FindWindow(windowClassName.c_str(), windowTitleName.c_str());
			if (handle != NULL)
			{
				lgr((Tstr)"应用程序已在运行" + windowTitleName + "\n", lm::wr);
				return 0;
			}
			return 1;
		}

#ifdef _WINDOWS
		//获取程序ID
		template<class T = bool>
		DWORD FindProcessIDByName(const Tstr& processName)
		{
			HANDLE hProcessSnap;
			PROCESSENTRY32 pe32;
			hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hProcessSnap == INVALID_HANDLE_VALUE)
			{
				return(0);
			}
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if (!Process32First(hProcessSnap, &pe32))
			{
				CloseHandle(hProcessSnap); // clean the snapshot object 清理快照对象
				return(0);
			}
			DWORD processId = 0;
			do
			{
				if ((Tstr)pe32.szExeFile == processName) //进程名称
				{
					processId = pe32.th32ProcessID; //进程ID
					break;
				}
			} while (Process32Next(hProcessSnap, &pe32));
			CloseHandle(hProcessSnap);
			return(processId);
		}
#endif

		//是否为管理员
		 bool IsUserAdmin();

		//获得管理员权限
		template<class T = bool>
		bool GainAdminPrivileges(Tstr strApp)
		{
			ShellMessage temp;

			if (!IsUserAdmin()) { //非管理员权限, 则申请
				ShellMessage UserAdmin("申请管理员权限", (int)ShellExecute(NULL, "runas", strApp.c_str(), NULL, NULL, SW_SHOWNORMAL));
				temp = UserAdmin;
			}
			//成功申请时, 退出当前进程
			if (temp.IsSucceed()) {
				return true;
			}

			return false;
		}


		/* 成功获取到管理员权限后, 返回 true
		* 同时, 可以将发出申请的程序退出, 使拥有管理员权限的程序单例运行
		* code:
		*	if (Typical_Tool::WindowsSystem::WindowHosting::获取管理员权限(true)) { return 0; }
		*/
		template<class T = bool>
		bool 获取管理员权限(bool isGet = true)
		{
			if (isGet) {
				//获取当前程序的全路径
				Tchar 程序路径[MAX_PATH] = "";
				GetModuleFileName(NULL, 程序路径, MAX_PATH);
				//获得管理员权限
				if (GainAdminPrivileges(程序路径)) {
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
		bool SetAutoRun(Tstr valueName, Tstr exePath)
		{
			LONG result;
			HKEY hKey;

			Tstr regPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

			// 打开注册表项  
			result = RegOpenKeyEx(HKEY_CURRENT_USER, regPath.c_str(), 0, KEY_SET_VALUE, &hKey);
			if (result != ERROR_SUCCESS) {
				lgc("打开密钥失败: %ld" + result, lm::er); // 假设 lgc 能够处理 wstring 和日志级别  
				return false;
			}

			// 设置注册表值  
			result = RegSetValueEx(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE*)exePath.c_str(), (Tstrlen(exePath.c_str()) + 1) * sizeof(Tchar));
			if (result != ERROR_SUCCESS) {
				lgc("设置注册表值失败: %ld" + result, lm::er); // 假设 lgc 能够处理 wstring 和日志级别 
				RegCloseKey(hKey);
				return false;
			}

			RegCloseKey(hKey);
			lgc("注册表注册成功!", lm::wr);
			return true;
		}

		//文件操作---------------------------------------------------------------------------------------------------------

		template<class T = bool>
		Tstr 提取程序名(const Tstr& path)
		{
			//匹配 '\' && '/' 任意
			size_t lastSepPos = path.find_last_of("\\/");
			if (lastSepPos != std::wstring::npos) {
				return path.substr(lastSepPos + 1); // 提取文件名部分
			}

			// 去掉 .exe 后缀
			size_t exePos = path.find_last_of(".exe");
			if (exePos != Tstr::npos && exePos == path.length() - 4) {
				path = path.substr(0, exePos); // 去掉 .exe 后缀
			}

			return path; // 如果找不到路径分隔符，则返回整个路径
		}

		template<class T = bool>
		Tstr 提取程序目录路径(const Tstr& path)
		{
			size_t lastSepPos = path.find_last_of("\\/");
			if (lastSepPos != std::wstring::npos) {
				//return path.substr(0, lastSepPos + 1); // 包括最后一个路径分隔符
				return path.substr(0, lastSepPos); // 不包括最后一个路径分隔符
			}
			return ""; // 如果找不到路径分隔符，则返回空字符串
		}
		template<class T = bool>
		Tstr Get程序名()
		{
			Tchar exePath[MAX_PATH];
			Tstr exeName;

			//获取当前程序的全路径
			DWORD length = GetModuleFileName(NULL, exePath, MAX_PATH);

			if (length > 0 && length < MAX_PATH) {
				exeName = 提取程序名(exePath);
				lgc(_T("当前可执行文件的名称: " + exeName));
				lgc();
			}
			else {
				lgc("无法获取当前可执行文件的路径!");
				lgc();
			}
			return exeName;
		}
		template<class T = bool>
		Tstr Get程序目录路径()
		{
			Tchar exePath[MAX_PATH];
			Tstr folderName;

			//获取当前程序的全路径
			DWORD length = GetModuleFileName(NULL, exePath, MAX_PATH);

			if (length > 0 && length < MAX_PATH) {
				folderName = 提取程序目录路径(exePath);
				lgc(_T("当前程序目录路径名: " + folderName));
				lgc();
			}
			else {
				lgc("无法获取当前可执行文件的路径!");
				lgc();
			}

			return folderName;
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
					lgc("文件夹: " + folderPath + " 创建成功!", lm::ts);
					lgc();
					return true;
				}
				lgc("文件夹: " + folderPath + " 创建失败!", lm::ts);
				lgc();
				// 创建失败且不是因为路径已存在  
				return false;
			}
			else if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				lgc("文件夹: " + folderPath + " 已存在", lm::ts);
				lgc();
				// 路径已经是一个目录  
				return true;
			}
			lgc("文件夹: " + folderPath + " 创建失败(路径存在, 但不是目录)!", lm::ts);
			lgc();
			// 路径存在但不是目录（可能是一个文件）  
			return false;
		}
		template<class T = bool>
		void OpenFolder(const Tstr& path)
		{
			ShellExecute(NULL, NULL, path.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}

		//控制台----------------------------------------------------------------------------------------------------------

		//移动光标到目标位置
		 void MoveCursorLocation(int x, int y);
	}
	namespace WinSys = WindowsSystem;

	//字符处理---------------------------------------------------------------------------------------------------------------
	namespace StringManage 
	{

		//字符转换-------------------------------------------------------------------------------------------------------
		 std::wstring StringToWstring(const std::string& str);
		 std::wstring StringToWstring(std::string&& str);
#define stow StringToWstring
		 std::string WstringToString(const std::wstring& wstr);
		 std::string WstringToString(std::wstring&& wstr);
#define wtos WstringToString

		//编码转换---------------------------------------------------------------------------------------------------------
	}
	namespace 字符处理 = StringManage;

	namespace GameTools
	{
		//帧率-----------------------------------------------------------------------------------------------------------

#ifndef FPS_COUNT
#define FPS_COUNT 60 //Fps 统计的间隔(帧率)
#endif
		//获取 FPS
		 float GetFps();
	}
	namespace 游戏工具 = GameTools;
}

#endif