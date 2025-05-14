#include <TypicalTool/Tool/pch.h>

#include <TypicalTool/Tool/Log.h>


//控制台初始化
//bool Typical_Tool::Log::LogInitialize		= false;
//bool Typical_Tool::Log::IsLogFileWrite		= false;
//bool Typical_Tool::Log::IsLogAllOutput		= false;
//bool Typical_Tool::Log::ShowLog				= true;
//bool Typical_Tool::Log::ShowTime			= true;
//bool Typical_Tool::Log::SingleLogFile		= true;
//bool Typical_Tool::Log::Debug				= false;
//bool Typical_Tool::Log::bFlushConsoleOutput = false;
//int64_t Typical_Tool::Log::PauseTime		= 200LL;
//std::atomic<bool> Typical_Tool::Log::IsLogFileWriteThreadStop = true;




void tytl::Log::SetShowLog(bool _ShowLog)
{
	bShowLog = _ShowLog;
}

void tytl::Log::SetShowTime(bool _ShowTime)
{
	bShowTime = _ShowTime;
}

void tytl::Log::SetShowConsole(bool _ShowConsole)
{
#ifdef _WINDOWS
	//显示/隐藏 窗口
	if (_ShowConsole) {
		ShowWindow(hConsole, SW_SHOWDEFAULT);
	}
	else {
		ShowWindow(hConsole, SW_HIDE);
	}
#else
	if (Debug) {
		Log_Out(ANSIESC_RED, Terr, Printf(TEXT("%s%s%s"), Log_Error, TEXT("%sLog::SetShowConsole: 显示控制台: 无效(#ifndef _WINDOWS)\n"), Log_LineFeed), ANSIESC_RESET, -1);
	}
#endif
}

void tytl::Log::SetDebug(const bool& _Debug)
{
	Debug = _Debug;
}

void tytl::Log::EnableAnsiEscape() {
#ifdef _WINDOWS
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	if (GetConsoleMode(hOut, &dwMode)) {
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	}
#endif
}