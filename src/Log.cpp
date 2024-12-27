#include "pch.h"

#include "Log.h"


//控制台初始化
bool Typical_Tool::Log::init = false;
Typical_Tool::LogMessage Typical_Tool::Log::LastMessage = lm::end;

bool Typical_Tool::Log::IsLogFileWrite = false;
bool Typical_Tool::Log::IsLogAllOutput = false;


Typical_Tool::Log::~Log()
{
	//当程序结束时, 退出 日志文件写入线程
	StopLogWrite();

	if (LogFileStream_Out.good()) {
		LogFileStream_Out.close();
	}
}

Typical_Tool::LogMessage Typical_Tool::ts()
{
	return LogMessage::tip;
}

Typical_Tool::LogMessage Typical_Tool::wr()
{
	return LogMessage::war;
}

Typical_Tool::LogMessage Typical_Tool::er()
{
	return LogMessage::err;
}

Typical_Tool::LogMessage Typical_Tool::ed()
{
	return LogMessage::end;
}

Typical_Tool::LogMessage Typical_Tool::lf()
{
	return LogMessage::lnf;
}