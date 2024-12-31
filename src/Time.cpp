#include "pch.h" 

#include "Time_Typical.h"


bool Typical_Tool::Time::IsShowLog = true;



void Typical_Tool::Time::SetShowLog(bool _IsShowLog)
{
	IsShowLog = _IsShowLog;
}

std::chrono::steady_clock::time_point Typical_Tool::Timer::GetTime()
{
	return std::chrono::steady_clock::now(); // 获取当前时间
}

void Typical_Tool::Timer::AddTimer()
{
	if (!this->IsSaveAllTimePoint) {
		this->TimerContainer[1] = this->TimerContainer[2];
		this->TimerContainer[2] = GetTime();
		return;
	}

	this->TimerContainer.push_back(GetTime());
}
void Typical_Tool::Timer::AddTimer(const std::chrono::steady_clock::time_point& _TimePoint)
{
	if (!this->IsSaveAllTimePoint) {
		this->TimerContainer[1] = this->TimerContainer[2];
		this->TimerContainer[2] = _TimePoint;
		return;
	}

	this->TimerContainer.push_back(_TimePoint);
}
void Typical_Tool::Timer::AddTimer(std::chrono::steady_clock::time_point&& _TimePoint)
{
	if (!this->IsSaveAllTimePoint) {
		this->TimerContainer[1] = this->TimerContainer[2];
		this->TimerContainer[2] = _TimePoint;
		return;
	}

	this->TimerContainer.push_back(_TimePoint);
}

void Typical_Tool::Timer::SetTimer(const std::chrono::steady_clock::time_point& _TimePoint, int _Location)
{
	Time_IsValid_RunTime(_Location, "SetTimer()");

	this->TimerContainer[_Location] = _TimePoint;
}

void Typical_Tool::Timer::SetTimer(std::chrono::steady_clock::time_point&& _TimePoint, int _Location)
{
	Time_IsValid_RunTime(_Location, "SetTimer()");
	
	this->TimerContainer[_Location] = _TimePoint;
}

std::chrono::steady_clock::time_point Typical_Tool::Timer::GetTimer(int _Location)
{
	Time_IsValid_RunTime(_Location, "GetTimer()");

	return this->TimerContainer[_Location];
}

std::deque<std::chrono::steady_clock::time_point> Typical_Tool::Timer::GetTimerContainer()
{
	return this->TimerContainer;
}

int Typical_Tool::Timer::GetTimerSize()
{
	return (int)this->TimerContainer.size();
}

