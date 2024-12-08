#include "pch.h" 

#include "Timers.h"

bool Typical_Tool::Timers::showLog = false;

void Typical_Tool::Timers::SetShowLog(bool _showLog)
{
	showLog = _showLog;
}

std::chrono::system_clock::time_point Typical_Tool::Timers::GetTime()
{
	return std::chrono::system_clock::now(); // 获取当前时间
}
std::chrono::steady_clock::time_point Typical_Tool::Timers::GetTime_s()
{
	return std::chrono::high_resolution_clock::now(); // 获取当前时间
}

void Typical_Tool::Timers::AddTimer()
{
	this->TimerContainer.push_back(GetTime());
}
void Typical_Tool::Timers::AddTimer(const std::chrono::system_clock::time_point& time)
{
	this->TimerContainer.push_back(time);
}
void Typical_Tool::Timers::AddTimer(std::chrono::system_clock::time_point&& time)
{
	this->TimerContainer.push_back(time);
}
void Typical_Tool::Timers::AddTimer_s()
{
	this->TimerContainer_s.push_back(GetTime_s());
}
void Typical_Tool::Timers::AddTimer_s(const std::chrono::steady_clock::time_point& time)
{
	this->TimerContainer_s.push_back(time);
}
void Typical_Tool::Timers::AddTimer_s(std::chrono::steady_clock::time_point&& time)
{
	this->TimerContainer_s.push_back(time);
}

void Typical_Tool::Timers::SetTimer(const std::chrono::system_clock::time_point& time, int Location)
{
	this->TimerContainer[Location] = time;
}
void Typical_Tool::Timers::SetTimer(std::chrono::system_clock::time_point&& time, int Location)
{
	this->TimerContainer[Location] = time;
}
void Typical_Tool::Timers::SetTimer_s(const std::chrono::steady_clock::time_point& time, int Location)
{
	this->TimerContainer_s[Location] = time;
}
void Typical_Tool::Timers::SetTimer_s(std::chrono::steady_clock::time_point&& time, int Location)
{
	this->TimerContainer_s[Location] = time;
}

std::chrono::system_clock::time_point Typical_Tool::Timers::GetTimer(int Location)
{
	return this->TimerContainer[Location];
}
std::chrono::steady_clock::time_point Typical_Tool::Timers::GetTimer_s(int Location)
{
	return this->TimerContainer_s[Location];
}

long long Typical_Tool::Timers::ComputationFirstToEndInterval()
{
	// first - end
	if (!this->HighPrecision) {
		return std::chrono::duration_cast<std::chrono::seconds>(
			this->TimerContainer[this->TimerContainer.size() - 1] - this->TimerContainer[0]).count();
	}
	else {
		switch (this->timeMeasure)
		{
		case tms::ms:
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(
				this->TimerContainer_s[this->TimerContainer_s.size() - 1] - this->TimerContainer_s[0]).count();
		}
		case tms::us:
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(
				this->TimerContainer_s[this->TimerContainer_s.size() - 1] - this->TimerContainer_s[0]).count();
		}
		case tms::ns:
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(
				this->TimerContainer_s[this->TimerContainer_s.size() - 1] - this->TimerContainer_s[0]).count();
		}
		break;
		}
	}
}

long long Typical_Tool::Timers::ComputationFrontToBackInterval()
{
	// front - end
	if (!this->HighPrecision) {
		return std::chrono::duration_cast<std::chrono::seconds>(
			this->TimerContainer[this->TimerContainer.size() - 1] - this->TimerContainer[this->TimerContainer.size() - 2]).count();
	}
	else {
		switch (this->timeMeasure)
		{
		case tms::ms:
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(
				this->TimerContainer_s[this->TimerContainer_s.size() - 1] - this->TimerContainer_s[this->TimerContainer_s.size() - 2]).count();
		}
		case tms::us:
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(
				this->TimerContainer_s[this->TimerContainer_s.size() - 1] - this->TimerContainer_s[this->TimerContainer_s.size() - 2]).count();
		}
		case tms::ns:
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(
				this->TimerContainer_s[this->TimerContainer_s.size() - 1] - this->TimerContainer_s[this->TimerContainer_s.size() - 2]).count();
		}
		break;
		}
	}
}

long long Typical_Tool::Timers::ComputationInterval(int LocationBegin, int LocationEnd)
{
	if (LocationBegin < 0) {
		throw std::runtime_error("Typical_Tool::Timers::ComputationInterval(): LocationBegin < 0");

		return 0;
	}
	else if (LocationEnd > this->TimerContainer.size() - 1) {
		throw std::runtime_error("Typical_Tool::Timers::ComputationInterval(): LocationEnd > TimerContainer.size() - 1");

		return 0;
	}

	// front - end
	if (!this->HighPrecision) {
		return std::chrono::duration_cast<std::chrono::seconds>(
			this->TimerContainer[LocationEnd] - this->TimerContainer[LocationBegin]).count();
	}
	else {
		switch (this->timeMeasure)
		{
		case tms::ms:
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(
				this->TimerContainer_s[LocationEnd] - this->TimerContainer_s[LocationBegin]).count();
		}
		case tms::us:
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(
				this->TimerContainer_s[LocationEnd] - this->TimerContainer_s[LocationBegin]).count();
		}
		case tms::ns:
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(
				this->TimerContainer_s[LocationEnd] - this->TimerContainer_s[LocationBegin]).count();
		}
		break;
		}
	}
}

bool Typical_Tool::Timers::TransformTimes(long long& time, TimeMeasure tms, TimeMeasure transformLaterTms)
{
	bool isSucceed = false;

	switch (tms)
	{
	case TimeMeasure::sec:
	{
		switch (transformLaterTms)
		{
		case TimeMeasure::sec:
		{
			//time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(time)).count();
			isSucceed = false;
			break;
		}
		case TimeMeasure::ms:
		{
			time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::us:
		{
			time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ns:
		{
			time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(time)).count();
			isSucceed = true;
			break;
		}
		}

		break;
	}
	case TimeMeasure::ms:
	{
		switch (transformLaterTms)
		{
		case TimeMeasure::sec:
		{
			time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ms:
		{
			//time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::milliseconds(time)).count();
			isSucceed = false;
			break;
		}
		case TimeMeasure::us:
		{
			time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::milliseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ns:
		{
			time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(time)).count();
			isSucceed = true;
			break;
		}
		}

		break;
	}
	case TimeMeasure::us:
	{
		switch (transformLaterTms)
		{
		case TimeMeasure::sec:
		{
			time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::microseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ms:
		{
			time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::microseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::us:
		{
			//time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::microseconds(time)).count();
			isSucceed = false;
			break;
		}
		case TimeMeasure::ns:
		{
			time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::microseconds(time)).count();
			isSucceed = true;
			break;
		}
		}

		break;
	}
	case TimeMeasure::ns:
	{
		switch (transformLaterTms)
		{
		case TimeMeasure::sec:
		{
			time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ms:
		{
			time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::us:
		{
			time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::nanoseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ns:
		{
			//time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::nanoseconds(time)).count();
			isSucceed = false;
			break;
		}
		}

		break;
	}
	}

	return isSucceed;
}
long long Typical_Tool::Timers::TransformTimes(const long long& time, bool& isSucceed, TimeMeasure tms, TimeMeasure transformLaterTms)
{
	long long temp = 0L;

	switch (tms)
	{
	case TimeMeasure::sec:
	{
		switch (transformLaterTms)
		{
		case TimeMeasure::sec:
		{
			//time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(time)).count();
			isSucceed = false;
			break;
		}
		case TimeMeasure::ms:
		{
			temp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::us:
		{
			temp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ns:
		{
			temp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(time)).count();
			isSucceed = true;
			break;
		}
		}

		break;
	}
	case TimeMeasure::ms:
	{
		switch (transformLaterTms)
		{
		case TimeMeasure::sec:
		{
			temp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ms:
		{
			//time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::milliseconds(time)).count();
			isSucceed = false;
			break;
		}
		case TimeMeasure::us:
		{
			temp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::milliseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ns:
		{
			temp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(time)).count();
			isSucceed = true;
			break;
		}
		}

		break;
	}
	case TimeMeasure::us:
	{
		switch (transformLaterTms)
		{
		case TimeMeasure::sec:
		{
			temp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::microseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ms:
		{
			temp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::microseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::us:
		{
			//time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::microseconds(time)).count();
			isSucceed = false;
			break;
		}
		case TimeMeasure::ns:
		{
			temp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::microseconds(time)).count();
			isSucceed = true;
			break;
		}
		}

		break;
	}
	case TimeMeasure::ns:
	{
		switch (transformLaterTms)
		{
		case TimeMeasure::sec:
		{
			temp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ms:
		{
			temp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::us:
		{
			temp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::nanoseconds(time)).count();
			isSucceed = true;
			break;
		}
		case TimeMeasure::ns:
		{
			//time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::nanoseconds(time)).count();
			isSucceed = false;
			break;
		}
		}

		break;
	}
	}

	return time;
}

void Typical_Tool::Timers::sleep_s(long long ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));

	if (showLog) {
		lgc("暂停: [" + Tto_string(ms) + "]毫秒", lm::ts);
	}
}

std::vector<std::chrono::system_clock::time_point> Typical_Tool::Timers::GetTimerContainer()
{
	return this->TimerContainer;
}
std::vector<std::chrono::steady_clock::time_point> Typical_Tool::Timers::GetTimerContainer_s()
{
	return this->TimerContainer_s;
}

int Typical_Tool::Timers::GetTimerSize()
{
	return this->TimerContainer.size();
}

int Typical_Tool::Timers::GetTimerSize_s()
{
	return this->TimerContainer_s.size();
}
