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
			static Shell消息 ExecuteAnalyze(Tstr 操作名, Tstr Shell操作, Tstr Shell文件, Tstr Shell参数 = "", Tstr 窗口显示 = "是");
		};
		using WinShell = WindowShell;
	}
}

#endif