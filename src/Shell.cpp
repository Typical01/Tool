#include "pch.h"
#include "Shell.h"



void Typical_Tool::WindowsSystem::WindowShell::Shell处理(HMENU 菜单, std::vector<ShellConfig>& Shell配置)
{
	lgc();
	lgc("Typical_Tool::WindowsSystem::WindowShell::Shell处理");
	lgc();

	for (auto tempShell = Shell配置.begin(); tempShell != Shell配置.end(); tempShell++) {
		//判断类型
		Tstr 操作名 = tempShell->操作名;
		bool 菜单按键 = tempShell->菜单按键;

		//区分: 程序启动项/程序菜单项s
		if (菜单按键) {
			程序启动项.push_back(*tempShell);
			lgc("操作名: " + 操作名);
			lgc();
			lgc("  注册: 程序启动项");
			lgc();
			tempShell->OutConfig(); //输出配置
		}
		else {
			int 菜单项ID = WinHost::GetHMENU();
			int 菜单项总数 = GetMenuItemCount(菜单);

			程序菜单项.insert(std::make_pair(菜单项ID, *tempShell));
			//插入菜单项
			InsertMenu(菜单, 菜单项总数 - 4, MF_STRING, 菜单项ID, 操作名.c_str());
			lgc("操作名: " + 操作名);
			lgc();
			lgc("  注册: 程序菜单项");
			tempShell->OutConfig(); //输出配置
			lgc();
		}
	}
}

void Typical_Tool::WindowsSystem::WindowShell::执行程序启动项Shell()
{
	//遍历执行所有: 程序启动项
	if (程序启动项.size() != 0) {
		for (auto tempShell = 程序启动项.begin(); tempShell != 程序启动项.end(); tempShell++) {
			auto 操作名 = tempShell->操作名;
			auto Shell操作 = tempShell->Shell操作;
			auto 文件 = tempShell->文件;
			auto 参数 = tempShell->参数;
			auto 窗口显示 = tempShell->窗口显示;

			if (Shell操作 == "打开文件") {
				执行_打开文件(操作名, 文件, 参数, 窗口显示);
			}
			else if (Shell操作 == "打开文件夹") {
				执行_打开文件夹(操作名, 文件, 参数, 窗口显示);
			}
			else if (Shell操作 == "管理员运行") {
				执行_管理员运行(操作名, 文件, 参数, 窗口显示);
			}
			else {
				lgc("配置模式错误(打开文件/打开文件夹/管理员运行): ", 操作名);
				lgc();
			}
		}
	}
	else {
		lgc("程序启动项Shell: 没有执行项!", lm::wr);
		lgc();
	}
}

void Typical_Tool::WindowsSystem::WindowShell::执行程序菜单项Shell(int _菜单选项ID)
{
	//查找并执行对应菜单ID的 ShellConfig
	auto temp = 程序菜单项.find(_菜单选项ID);
	if (temp != 程序菜单项.end()) {
		ShellConfig tempShellConfig = temp->second;

		auto 操作名 = tempShellConfig.操作名;
		auto Shell操作 = tempShellConfig.Shell操作;
		auto 文件 = tempShellConfig.文件;
		auto 参数 = tempShellConfig.参数;
		auto 窗口显示 = tempShellConfig.窗口显示;

		if (Shell操作 == "打开文件") {
			执行_打开文件(操作名, 文件, 参数, 窗口显示);
		}
		else if (Shell操作 == "打开文件夹") {
			执行_打开文件夹(操作名, 文件, 参数, 窗口显示);
		}
		else if (Shell操作 == "管理员运行") {
			执行_管理员运行(操作名, 文件, 参数, 窗口显示);
		}
		else {
			lgc("配置模式错误(打开文件/打开文件夹/管理员运行): ", 操作名);
			lgc();
		}
	}
	else {
		lgc("程序菜单项Shell: 没有找到菜单选项 " + _菜单选项ID, lm::er);
		lgc();
	}
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::执行(Tstr 操作名, Tstr Shell操作, Tstr Shell文件, Tstr Shell参数, int 窗口显示)
{
	Shell消息 temp(操作名, (int)ShellExecute(NULL, Shell操作.c_str(), Shell文件.c_str(), Shell参数.c_str(), NULL, 窗口显示));
	return temp;
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::执行_管理员运行(Tstr 操作名, Tstr Shell文件, Tstr Shell参数, int 窗口显示)
{
	return 执行(操作名, "runas", Shell文件, Shell参数, 窗口显示);
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::执行_打开文件夹(Tstr 操作名, Tstr Shell文件, Tstr Shell参数, int 窗口显示)
{
	return 执行(操作名, "explore", Shell文件, Shell参数, 窗口显示);
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::执行_打开文件(Tstr 操作名, Tstr Shell文件, Tstr Shell参数, int 窗口显示)
{
	return 执行(操作名, "open", Shell文件, Shell参数, 窗口显示);
}

void Typical_Tool::WindowsSystem::ShellConfig::OutConfig()
{
	lgc();
	lgc();
	lgc("ShellConfig::OutConfig()", lm::ts);
	lgc();
	lgc("操作名: " + this->操作名);
	lgc();
	lgc("菜单按键: " + this->菜单按键);
	lgc();
	lgc("Shell操作: " + this->Shell操作);
	lgc();
	lgc("文件: " + this->文件);
	lgc();
	lgc("参数: " + this->参数);
	lgc();
	lgc("窗口显示: " + this->窗口显示);
	lgc();
	lgc();
}
