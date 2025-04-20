#include <libTypical/Tool/pch.h>

#include <libTypical/Tool/Time_Typical.h>


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
	std::lock_guard<std::mutex> tempMutex(this->mutex_Timer);

	if (!this->IsSaveAllTimePoint) {
		this->TimerContainer[1] = this->TimerContainer[2];
		this->TimerContainer[2] = GetTime();
		return;
	}

	this->TimerContainer.push_back(GetTime());
}
void Typical_Tool::Timer::AddTimer(const std::chrono::steady_clock::time_point& _TimePoint)
{
	std::lock_guard<std::mutex> tempMutex(this->mutex_Timer);

	if (!this->IsSaveAllTimePoint) {
		this->TimerContainer[1] = this->TimerContainer[2];
		this->TimerContainer[2] = _TimePoint;
		return;
	}

	this->TimerContainer.push_back(_TimePoint);
}
void Typical_Tool::Timer::AddTimer(std::chrono::steady_clock::time_point&& _TimePoint)
{
	std::lock_guard<std::mutex> tempMutex(this->mutex_Timer);

	if (!this->IsSaveAllTimePoint) {
		this->TimerContainer[1] = this->TimerContainer[2];
		this->TimerContainer[2] = _TimePoint;
		return;
	}

	this->TimerContainer.push_back(_TimePoint);
}

std::vector<std::chrono::steady_clock::time_point> Typical_Tool::Timer::GetTimerContainer()
{
	std::lock_guard<std::mutex> tempMutex(this->mutex_Timer);
	return this->TimerContainer;
}

int Typical_Tool::Timer::GetTimerSize()
{
	std::lock_guard<std::mutex> tempMutex(this->mutex_Timer);
	return (int)this->TimerContainer.size();
}

