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
		Tstr 菜单按键 = tempShell->菜单按键;

		//区分: 程序启动项/程序菜单项s
		if (菜单按键 == "否") {
			程序启动项.push_back(*tempShell);
			lgc("操作名: " + 操作名);
			lgc("  注册: 程序启动项");
			tempShell->OutConfig(); //输出配置
		}
		else {
			int 菜单项ID = WinHost::GetHMENU();
			//int 菜单项总数 = GetMenuItemCount(菜单);

			程序菜单项.insert(std::make_pair(菜单项ID, *tempShell));
			lgc("操作名: " + 操作名);
			lgc("  注册: 程序菜单项");
			//添加菜单项
			if (AppendMenuW(菜单, MF_STRING, 菜单项ID, StringManage::stow(操作名).c_str())) {
				tempShell->OutConfig(); //输出配置
				lgc("  程序菜单项: 成功");
			}
			else {
				lgc("  程序菜单项: 失败");
			}
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

			ExecuteAnalyze(操作名, Shell操作, 文件, 参数, 窗口显示);
		}
	}
	else {
		lgcr("程序启动项Shell: 没有执行项!", lm::wr);
		lgcr();
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

		ExecuteAnalyze(操作名, Shell操作, 文件, 参数, 窗口显示);
	}
	else {
		lgcr("程序菜单项Shell: 没有找到菜单选项 " + _菜单选项ID, lm::er);
		lgcr();
	}
}

Shell消息 Typical_Tool::WindowsSystem::WindowShell::ExecuteAnalyze(Tstr 操作名, Tstr Shell操作, Tstr Shell文件, Tstr Shell参数, Tstr 窗口显示)
{
	if (Shell操作 == "打开文件" || Shell操作 == "open") {
		Shell操作 = "open";
		lgc("ExecuteAnalyze: Shell操作模式(打开文件)", lm::ts);
	}
	else if (Shell操作 == "管理员运行" || Shell操作 == "runas") {
		Shell操作 = "runas";
		lgc("ExecuteAnalyze: Shell操作模式(管理员运行)", lm::ts);
	}
	else if (Shell操作 == "打开文件夹" || Shell操作 == "explore") {
		Shell操作 = "explore";
		lgc("ExecuteAnalyze: Shell操作模式(打开文件夹)", lm::ts);
	}
	else {
		lgcr("ExecuteAnalyze: Shell操作模式错误(打开文件/打开文件夹/管理员运行)", lm::wr);
		lgcr("ExecuteAnalyze: 操作名: " + 操作名, lm::wr);
		return Shell消息();
	}

	int ShowWindow = 0;
	if (窗口显示 == "是") {
		ShowWindow = 5;
	}
	lgc("ExecuteAnalyze: 窗口显示 " + 窗口显示, lm::wr);

	Shell消息 temp(操作名, (int)ShellExecuteW(NULL,  stow(Shell操作).c_str(), stow(Shell文件).c_str(), stow(Shell参数).c_str(), NULL, ShowWindow));
	return temp;
}

void Typical_Tool::WindowsSystem::ShellConfig::OutConfig()
{
	lgc("ShellConfig::OutConfig()", lm::ts);
	lgc("操作名: " + this->操作名);
	lgc("菜单按键: " + this->菜单按键);
	lgc("Shell操作: " + this->Shell操作);
	lgc("文件: " + this->文件);
	lgc("参数: " + this->参数);
	lgc("窗口显示: " + this->窗口显示);
	lgc();
}
