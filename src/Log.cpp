#include "pch.h"

#include "Log.h"


//控制台初始化
bool Typical_Tool::Log::FirstInit = false;

#ifdef _WINDOWS
HWND Typical_Tool::hConsole = nullptr;
#endif
FILE* Typical_Tool::LogFileStream = nullptr;
bool Typical_Tool::LogFileWrite = false;
bool Typical_Tool::LogAllOutput = false;

std::queue<Ustr> Typical_Tool::LogFileWrite_Queue; //日志文件写入队列
std::thread Typical_Tool::LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件

bool Typical_Tool::LogFileWriteThreadStop = false;
std::mutex Typical_Tool::mutex_LogFileStream;

std::vector<Typical_Tool::Log> Typical_Tool::Log_All = { Typical_Tool::lg, Typical_Tool::lgc, Typical_Tool::lgr, Typical_Tool::lgcr };



Typical_Tool::Log::~Log()
{
	//当程序结束时, 退出 日志文件写入线程
	LogFileWriteThreadStop = true;

	if (LogFileStream != nullptr) {
		fclose(LogFileStream);
	}
}

void Typical_Tool::Log::SetShowLog(bool showLog)
{
	this->ShowLog = showLog;
}

void Typical_Tool::Log::SetShowTime(bool showTime)
{
	this->ShowTime = showTime;
}

void Typical_Tool::Log::SetShowConsole(bool showConsole, bool isTips)
{
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