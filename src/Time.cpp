#include "pch.h" 

#include "Time_Typical.h"


bool Typical_Tool::Time::showLog = true;
bool Typical_Tool::Timer::showLog = true;



void Typical_Tool::Timer::SetShowLog(bool _showLog)
{
	showLog = _showLog;
}
void Typical_Tool::Time::SetShowLog(bool _showLog)
{
	showLog = _showLog;
}

std::chrono::steady_clock::time_point Typical_Tool::Timer::GetTime()
{
	return std::chrono::steady_clock::now(); // 获取当前时间
}

void Typical_Tool::Timer::AddTimer()
{
	this->TimerContainer.push_back(GetTime());
}
void Typical_Tool::Timer::AddTimer(const std::chrono::steady_clock::time_point& time)
{
	this->TimerContainer.push_back(time);
}
void Typical_Tool::Timer::AddTimer(std::chrono::steady_clock::time_point&& time)
{
	this->TimerContainer.push_back(time);
}

void Typical_Tool::Timer::SetTimer(const std::chrono::steady_clock::time_point& time, int Location)
{
	this->TimerContainer[Location] = time;
}
void Typical_Tool::Timer::SetTimer(std::chrono::steady_clock::time_point&& time, int Location)
{
	this->TimerContainer[Location] = time;
}

std::chrono::steady_clock::time_point Typical_Tool::Timer::GetTimer(int Location)
{
	return this->TimerContainer[Location];
}

long long Typical_Tool::Timer::ComputTime_FirstToEnd()
{
	// first - end
	return std::chrono::duration_cast<std::chrono::seconds>(
		this->TimerContainer[this->TimerContainer.size() - 1] - this->TimerContainer[0]).count();
}

long long Typical_Tool::Timer::ComputTime_FrontToBack()
{
	// front - end
	return std::chrono::duration_cast<std::chrono::seconds>(
		this->TimerContainer[this->TimerContainer.size() - 1] - this->TimerContainer[this->TimerContainer.size() - 2]).count();
}

long long Typical_Tool::Timer::ComputTime(int LocationBegin, int LocationEnd)
{
	if (LocationBegin < 0) {
		throw std::runtime_error("Typical_Tool::Timer::ComputationInterval(): LocationBegin < 0");

		return 0;
	}
	else if (LocationEnd > this->TimerContainer.size() - 1) {
		throw std::runtime_error("Typical_Tool::Timer::ComputationInterval(): LocationEnd > TimerContainer.size() - 1");

		return 0;
	}

	// front - end
	return std::chrono::duration_cast<std::chrono::seconds>(
		this->TimerContainer[LocationEnd] - this->TimerContainer[LocationBegin]).count();
}

std::vector<std::chrono::steady_clock::time_point> Typical_Tool::Timer::GetTimerContainer()
{
	return this->TimerContainer;
}

int Typical_Tool::Timer::GetTimerSize()
{
	return (int)this->TimerContainer.size();
}

