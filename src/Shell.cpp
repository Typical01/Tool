#include "pch.h"
#include "Shell.h"



void Typical_Tool::WindowsSystem::WindowShell::Shell处理(HMENU 菜单, std::vector<ShellConfig>& Shell配置)
{
	for (auto tempShell = Shell配置.cbegin(); tempShell != Shell配置.cend(); tempShell++) {
		//判断类型
		Ustr 操作名 = tempShell->操作名;
		Ustr Shell操作 = tempShell->Shell操作;
		bool 程序启动时运行 = tempShell->程序启动时运行;
		
		//注册 程序启动时Shell 事件函数
		if (程序启动时运行) {
			if (Shell操作 == "管理员运行") {
				程序启动时.insert(std::make_pair(*tempShell, 执行_管理员运行));
			}
			else if (Shell操作 == "打开文件夹") {
				程序启动时.insert(std::make_pair(*tempShell, 执行_打开文件夹));
			}
			else if (Shell操作 == "打开文件") {
				程序启动时.insert(std::make_pair(*tempShell, 执行_打开文件));
			}
		}

		//非启动时运行, 则注册菜单项
		Ustr 文件 = tempShell->文件;
		Ustr 参数 = tempShell->参数;
		int 窗口显示 = tempShell->窗口显示;
		
		int 菜单项总数 = GetMenuItemCount(菜单);
		int 菜单项ID = WinHost::GetHMENU();
		//注册菜单
		if (Shell操作 == "管理员运行") {
			//关联 菜单项ID和 操作名
			菜单ID.insert(std::make_pair(菜单项ID, 操作名));
			//插入菜单项
			InsertMenu(菜单, 菜单项总数 - 4, MF_STRING, 菜单项ID, 操作名.c_str());
			程序菜单项.insert(std::make_pair(*tempShell, 执行_管理员运行));
		}
		else if (Shell操作 == "打开文件夹") {
			//关联 菜单项ID和 操作名
			菜单ID.insert(std::make_pair(菜单项ID, 操作名));
			//插入菜单项
			InsertMenu(菜单, 菜单项总数 - 4, MF_STRING, 菜单项ID, 操作名.c_str());
			程序菜单项.insert(std::make_pair(*tempShell, 执行_打开文件夹));
		}
		else if (Shell操作 == "打开文件") {
			//关联 菜单项ID和 操作名
			菜单ID.insert(std::make_pair(菜单项ID, 操作名));
			//插入菜单项
			InsertMenu(菜单, 菜单项总数 - 4, MF_STRING, 菜单项ID, 操作名.c_str());
			程序菜单项.insert(std::make_pair(*tempShell, 执行_打开文件));
		}
	}
}

void Typical_Tool::WindowsSystem::WindowShell::程序启动时Shell()
{
	for (auto tempShell = 程序启动时.cbegin(); tempShell != 程序启动时.cend(); tempShell++) {
		auto 操作名 = tempShell->first.操作名;
		auto Shell操作 = tempShell->first.Shell操作;
		auto 文件 = tempShell->first.文件;
		auto 参数 = tempShell->first.参数;
		auto 窗口显示 = tempShell->first.窗口显示;

		auto Shell消息 = tempShell->second(操作名, 文件, 参数, 窗口显示);
		//if (Shell消息.IsSucceed()) {}
	}
}

void Typical_Tool::WindowsSystem::WindowShell::程序菜单项Shell(int 菜单选项ID)
{
	//遍历菜单ID
	for (auto temp菜单ID = 菜单ID.cbegin(); temp菜单ID != 菜单ID.cend(); temp菜单ID++) {
		if (菜单选项ID == temp菜单ID->first) {
			for (auto tempShell = 程序菜单项.cbegin(); tempShell != 程序菜单项.cend(); tempShell++) {
				auto 操作名 = tempShell->first.操作名;

				//找到需要的操作后, 执行操作
				if (temp菜单ID->second == 操作名) {
					auto Shell操作 = tempShell->first.Shell操作;
					auto 文件 = tempShell->first.文件;
					auto 参数 = tempShell->first.参数;
					auto 窗口显示 = tempShell->first.窗口显示;

					auto Shell消息 = tempShell->second(操作名, 文件, 参数, 窗口显示);
					//if (Shell消息.IsSucceed()) {}
				}
			}
		}

		//菜单ID不匹配
		continue;
	}

	
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::执行(Ustr 操作名, Ustr Shell操作, Ustr Shell文件, Ustr Shell参数, int 窗口显示)
{
	//Shell消息 temp("nvidia-smi", (int)ShellExecute(NULL, "runas", "cmd", "nvidia-smi -lgc 1080", NULL, SW_SHOWNORMAL));
	Shell消息 temp(操作名, (int)ShellExecute(NULL, Shell操作.c_str(), Shell文件.c_str(), Shell参数.c_str(), NULL, 窗口显示));
	return temp;
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::执行_管理员运行(Ustr 操作名, Ustr Shell文件, Ustr Shell参数, int 窗口显示)
{
	return 执行(操作名, "runas", Shell文件, Shell参数, 窗口显示);
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::执行_打开文件夹(Ustr 操作名, Ustr Shell文件, Ustr Shell参数, int 窗口显示)
{
	return 执行(操作名, "explore", Shell文件, Shell参数, 窗口显示);
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::执行_打开文件(Ustr 操作名, Ustr Shell文件, Ustr Shell参数, int 窗口显示)
{
	return 执行(操作名, "open", Shell文件, Shell参数, 窗口显示);
}
