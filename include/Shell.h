#pragma once

#ifndef _SHELL_H
#define _SHELL_H

#include "pch.h"
//#include "Typical_Tool.h"
#include "Message.h"
#include "WindowHosting.h"


using Typical_Tool::WindowsSystem::Shell消息;

namespace Typical_Tool {

	namespace WindowsSystem {

		class  ShellConfig {
		public:
			Ustr 操作名;

			bool 程序启动时运行;
			Ustr Shell操作;
			Ustr 文件;
			Ustr 参数;
			bool 窗口显示;

			ShellConfig(Ustr 操作名, bool 程序启动时运行, Ustr Shell操作, Ustr 文件, Ustr 参数 = NULL, bool 窗口显示 = true)
				: 操作名(操作名), 程序启动时运行(程序启动时运行), Shell操作(Shell操作), 文件(文件), 参数(参数), 窗口显示(窗口显示)
			{}

			bool operator<(const ShellConfig& other) const
			{
				if (操作名 != other.操作名) {
					return 操作名 < other.操作名;
				}
			}
		};

		class  WindowShell {
		private:
			//std::map<Ustr, ShellConfig> 程序启动时;
			std::map<ShellConfig, Shell消息(*)(Ustr, Ustr, Ustr, int)> 程序启动时;

			std::map<int, Ustr> 菜单ID;
			std::map<ShellConfig, Shell消息(*)(Ustr, Ustr, Ustr, int)> 程序菜单项;

		public:
			WindowShell() {
				
			}

			void Shell处理(HMENU 菜单, std::vector<ShellConfig>& Shell配置);
			void 程序启动时Shell();
			void 程序菜单项Shell(int 菜单选项ID);

			// 窗口显示: 0(SW_SHOW) 隐藏窗口
			/* 
			* Shell操作: runas / open / explore
			* Shell文件: cmd, note
			* Shell参数: /k
			*/
			static Shell消息 执行(Ustr 操作名, Ustr Shell操作, Ustr Shell文件, Ustr Shell参数 = NULL, int 窗口显示 = 5);

			static Shell消息 执行_管理员运行(Ustr 操作名, Ustr Shell文件, Ustr Shell参数 = NULL, int 窗口显示 = 5);
			static Shell消息 执行_打开文件夹(Ustr 操作名, Ustr Shell文件, Ustr Shell参数 = NULL, int 窗口显示 = 5);
			static Shell消息 执行_打开文件(Ustr 操作名, Ustr Shell文件, Ustr Shell参数 = NULL, int 窗口显示 = 5);
		};
		using WinShell = WindowShell;
	}
}

#endif