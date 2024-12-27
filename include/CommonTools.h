#pragma once

//常用工具
#ifndef _COMMONTOOLS_H
#define _COMMONTOOLS_H

#include "pch.h"
#include "Log.h"
#include "Time_Typical.h"
#include "WindowHost.h"

using namespace std;

namespace Typical_Tool {

	//Windows系统操作----------------------------------------------------------------------------------------
	namespace WindowsSystem
	{

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
				lgc("Windows DPI: 传入的值无效\n");
			}
			else {
				lgc("Windows DPI: DPI感知(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE) 设置成功!\n");
				
			}
		}


		template<class T = bool>
		int AloneRun(Tstr windowClassName, Tstr windowTitleName)
		{
			//程序启动初始化
			HWND handle = FindWindow(windowClassName.c_str(), windowTitleName.c_str());
			if (handle != NULL)
			{
				lgr((Tstr)"应用程序已在运行" + windowTitleName + "\n", wr);
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
				ShellMessage UserAdmin("申请管理员权限", (int)ShellExecute(NULL, "runas", strApp.c_str(), NULL, NULL, SW_SHOWNORMAL));
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
		*	if (Typical_Tool::WindowsSystem::WindowHost::获取管理员权限(true)) { return 0; }
		*/
		template<class T = bool>
		bool GetAdmin(bool isGet = true)
		{
			if (isGet) {
				//获取当前程序的全路径
				Tchar 程序路径[MAX_PATH] = _T("");
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
		bool SetSelfStarting(Tstr valueName, Tstr exePath)
		{
			LONG result;
			HKEY hKey;

			wstring regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

			// 打开注册表项  
			result = RegOpenKeyExW(HKEY_CURRENT_USER, regPath.c_str(), 0, KEY_SET_VALUE, &hKey);
			if (result != ERROR_SUCCESS) {
				lgc("打开密钥失败: %ld" + result, er);
				return false;
			}

			// 设置注册表值  
			result = RegSetValueExW(hKey, stow(valueName).c_str(), 0, REG_SZ, (const BYTE*)stow(exePath).c_str(), (stow(exePath).size() + 1) * sizeof(wchar_t));
			if (result != ERROR_SUCCESS) {
				lgc("设置注册表值失败: %ld" + result, er);
				RegCloseKey(hKey);
				return false;
			}

			RegCloseKey(hKey);
			lgc("注册表注册成功!", ts);
			return true;
		}

		//文件操作---------------------------------------------------------------------------------------------------------

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


		template<class T = bool>
		bool ExtractExeDirectoryName(Tstr& path)
		{
			size_t lastSepPos = path.find_last_of(_T("\\"));
			if (lastSepPos != std::wstring::npos) {
				path = path.substr(0, lastSepPos); // 不包括最后一个路径分隔符
				return true;
			}
			return false; // 如果找不到路径分隔符，则返回空字符串
		}

		template<class T = bool>
		bool GetExeName(Tstr& _ExeName)
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
				lgc("无法获取当前可执行文件的路径!");
				return false;
			}
		}

		template<class T = bool>
		bool GetExeDirectoryName(Tstr& _FolderName)
		{
			Tchar exePath[MAX_PATH];

			//获取当前程序的全路径
			DWORD length = GetModuleFileName(NULL, exePath, MAX_PATH);
			_FolderName = exePath;
			if (length > 0 && length < MAX_PATH) {
				if (ExtractExeDirectoryName(_FolderName)) {
					lgc(_T("当前程序目录路径名: ") + _FolderName);
				}
				return true;
			}
			else {
				lgc("无法获取当前可执行文件的路径!");
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
					lgc("文件夹: " + folderPath + " 创建成功!", ts);
					
					return true;
				}
				lgc("文件夹: " + folderPath + " 创建失败!", ts);
				
				// 创建失败且不是因为路径已存在  
				return false;
			}
			else if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				lgc("文件夹: " + folderPath + " 已存在", ts);
				// 路径已经是一个目录  
				return true;
			}
			lgc("文件夹: " + folderPath + " 创建失败(路径存在, 但不是目录)!", ts);
			// 路径存在但不是目录（可能是一个文件）  
			return false;
		}

		template<class T = bool>
		void OpenFolder(const Tstr& path)
		{
			ShellMessage OpenFolder("打开文件夹", (int)ShellExecute(NULL, NULL, path.c_str(), NULL, NULL, SW_SHOWNORMAL));
			if (!OpenFolder.IsSucceed()) {
				lg(_T("ShellExecute: 打开文件夹 失败!"), er);
			}
		}

		//控制台----------------------------------------------------------------------------------------------------------

		//移动光标到目标位置
		void MoveCursorLocation(int x, int y);
	}
	namespace WinSys = WindowsSystem;

	namespace GameTools
	{
		//帧率-----------------------------------------------------------------------------------------------------------

#ifndef FPS_COUNT
#define FPS_COUNT 60 //Fps 统计的间隔(帧率)
#endif
		//获取 FPS
		float GetFps();
	}
}

#endif