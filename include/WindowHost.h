#pragma once

#ifndef _WINDOWSHOSTING_H
#define _WINDOWSHOSTING_H

#include "pch.h"
#include "Log.h"
#include "StringManage.h"

using Typical_Tool::StringManage::stow;
using Typical_Tool::StringManage::wtos;

namespace Typical_Tool {
	namespace WindowsSystem {
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
				/*
				Font = CreateFont(
					-16, -7, 0, 0,
					400, //粗度 一般这个值设为400
					FALSE, //斜体
					FALSE, //下划线
					FALSE, //删除线
					DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
					FF_DONTCARE,
					"微软雅黑"
				);
				*/
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
					lgc("热键注册[" + this->信息 + "]: 成功👌");
					
				}
				else
				{
					lgc("热键注册[" + this->信息 + "]: 错误😒 -> 代码(" + To_string(message) + ")", er);
					
				}
			}
			RegisterHotKeyMessage(Tstr&& RegisterHotKey热键信息, int message)
				: 信息(RegisterHotKey热键信息)
			{
				if (message > 0)
				{
					lgc("热键注册[" + this->信息 + "]: 成功👌");
					
				}
				else
				{
					lgc("热键注册[" + this->信息 + "]: 错误😒 -> 代码(" + To_string(message) + ")", er);
					
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
					lgc("Shell消息[" + this->信息 + "]: 错误😒 -> 代码(" + To_string(message) + ")", er);
					
				}
				else
				{
					this->Status = true;
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 成功 👌");
					
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
					lgc("Shell消息[" + this->信息 + "]: 错误😒 -> 代码(" + To_string(message) + ")", er);
					
				}
				else
				{
					this->Status = true;
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 成功 👌");
					
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

			ShellConfig(Tstr _操作名, Tstr _Shell操作, Tstr _文件, Tstr _参数 = NULL, Tstr _窗口显示 = "是", Tstr _菜单按键 = "否")
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
			static ShellMessage ExecuteAnalyze(Tstr 操作名, Tstr Shell操作, Tstr Shell文件, Tstr Shell参数 = "", Tstr 窗口显示 = "是");
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
}

#endif