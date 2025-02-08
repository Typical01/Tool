#include <libTypical/Tool/pch.h>

#include <libTypical/Tool/Log.h>


//控制台初始化
bool Typical_Tool::Log::init = false;
bool Typical_Tool::Log::IsLogFileWrite = false;
bool Typical_Tool::Log::IsLogAllOutput = false;
bool Typical_Tool::Log::ShowLog = true;
bool Typical_Tool::Log::ShowTime = true;
bool Typical_Tool::Log::SingleLogFile = true;
bool Typical_Tool::Log::Debug = false;
bool Typical_Tool::Log::bFlushConsoleOutput = false;
long long Typical_Tool::Log::PauseTime = 200LL;
std::atomic<bool> Typical_Tool::Log::IsLogFileWriteThreadStop = true;


Typical_Tool::Log::~Log()
{
	//当程序结束时, 退出 日志文件写入线程
	StopLogWrite();
}
