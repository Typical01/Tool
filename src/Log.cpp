#include "pch.h"

#include "Log.h"


//控制台初始化
bool Typical_Tool::Log::FirstInit = false;

#ifdef _WINDOWS
HWND Typical_Tool::Log::hConsole = nullptr;
#endif
bool Typical_Tool::Log::LogFileWrite = false;
bool Typical_Tool::Log::LogAllOutput = false;

std::ofstream Typical_Tool::Log::LogFileStream_Out;

#ifdef _Thread
std::queue<Tstr> Typical_Tool::Log::LogFileWrite_Queue; //日志文件写入队列
std::thread Typical_Tool::Log::LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
std::atomic<bool> Typical_Tool::Log::LogFileWriteThreadStop{ false };
std::mutex Typical_Tool::Log::mutex_LogFileStream_Out;
std::condition_variable Typical_Tool::Log::cv_LogFileQueue;
#endif


Typical_Tool::Log::~Log()
{
	//当程序结束时, 退出 日志文件写入线程
	StopLogWrite();

	if (LogFileStream_Out.good()) {
		LogFileStream_Out.close();
	}
}

void Typical_Tool::Log::SetShowANSIESC(bool _showESC)
{
	if (_showESC) {
#define ANSI_RESET "\033[0m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_RED "\033[31m"
	}
	else {
#define ANSI_RESET ""
#define ANSI_GREEN ""
#define ANSI_YELLOW ""
#define ANSI_RED ""
	}
}

void Typical_Tool::Log::SetShowLog(bool showLog)
{
	Tcout << "Log 设置: 显示日志\n" << std::endl;

	this->ShowLog = showLog;
}

void Typical_Tool::Log::SetShowTime(bool showTime)
{
	Tcout << "Log 设置: 显示时间\n" << std::endl;

	this->ShowTime = showTime;
}

void Typical_Tool::Log::SetShowConsole(bool showConsole)
{
	Tcout << "Log 设置: 显示控制台\n" << std::endl;

#ifdef _WINDOWS
	//显示/隐藏 窗口
	if (showConsole) {
		ShowWindow(hConsole, SW_SHOWDEFAULT);
	}
	else {
		ShowWindow(hConsole, SW_HIDE);
	}
#endif
}