#pragma once

#ifndef _EDIT_H
#define _EDIT_H

#include "pch.h"
//#include "Typical_Tool.h"
#include "Log.h"
#include "WindowHosting.h"
#include "CharHandleOfConfigFile.h"
#include "ToolsConfigFile.h"


using namespace Typical_Tool::StringHandling;

namespace Typical_Tool {

	namespace WindowsSystem {

		class  WindowEdit {
		private:
			HWND hWnd_标签主窗口;
			HWND hWnd_标签选项卡;
			HWND hWnd_标签命名;
			HWND hWnd_标签命名_编辑框;
			HWND hWnd_标签命名_确认按键;
			int 标签选项卡计数;

			HWND hWnd_编辑窗口;
			std::map<Ustr, Ustr> hWnd_标签内容;
			TCITEMW 标签选项卡;

			int 主窗口宽;
			int 主窗口高;
		public:
			int 标签菜单_Add;
			int 标签菜单_Set;
			int 标签菜单_Del; 
			int 标签菜单_Help;
			int 标签菜单_Save;

			int 标签命名_编辑框;
			int 标签命名_确认按键;

		public:
			WindowEdit() { }

		public:

			void Init(CharHandleOfConfigFile& 配置文件);

			void 创建标签窗口(Ustr 标签窗口类名, WinHost& wh);
			void 显示标签窗口(int 显示窗口 = 5);
			void 显示命名窗口(int 显示窗口 = 5);
			
			void 添加标签();
			void 添加标签(Ustr 标签名, Ustr& 标签内容);
			void 添加标签(Ustr 标签名, Ustr&& 标签内容);
			void 修改标签();
			void 删除标签();
			Ustr Get标签名(int 标签计数);
			int Get当前标签选项();
			void 显示标签内容(Ustr 标签名);

			static bool 获取句柄文本(HWND hWnd, Ustr& 文本);
			int Get标签计数();

		public:
			void 标签窗口刷新();
			void 标签内容保存(ToolsConfigFile& 工具箱配置文件, CharHandleOfConfigFile& 配置文件);
		};
		using WinEdit = WindowEdit;
	}
}

#endif