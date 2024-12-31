#pragma once

//Timer.h 计时器
#ifndef _TIMERS_H_
#define _TIMERS_H_

#include "pch.h"
#include "Tchar_Typical.h"


namespace Typical_Tool {

	using day = std::chrono::duration<long long, std::ratio<60 * 60 * 24, 1>>; // 天
	using hour = std::chrono::duration<long long, std::ratio<60 * 60, 1>>;     // 时
	using min = std::chrono::duration<long long, std::ratio<60, 1>>;           // 分
	using sec = std::chrono::seconds;                                          // 秒
	using ms = std::chrono::milliseconds;                                      // 毫秒
	using us = std::chrono::microseconds;                                      // 微秒
	using ns = std::chrono::nanoseconds;                                       // 纳秒

	template <typename Target>
	Tstr TimeMeasureToString() {
		if constexpr (std::is_same<Target, day>::value) {
			return _T("天");
		}
		else if constexpr (std::is_same<Target, hour>::value) {
			return _T("时");
		}
		else if constexpr (std::is_same<Target, min>::value) {
			return _T("分");
		}
		else if constexpr (std::is_same<Target, sec>::value) {
			return _T("秒");
		}
		else if constexpr (std::is_same<Target, ms>::value) {
			return _T("毫秒");
		}
		else if constexpr (std::is_same<Target, us>::value) {
			return _T("微秒");
		}
		else if constexpr (std::is_same<Target, ns>::value) {
			return _T("纳秒");
		}
		else {
			return _T("未知单位");
		}
	}

	enum TimePointLocation {
		First = 0,	// 首 节点
		Front = 1,	// 前 节点
		Back = 2,	// 后 节点
		End = 2		// 尾 节点
	};
	using tpl = TimePointLocation;

	//计时器
	class Timer
	{
	private:
		std::chrono::steady_clock::time_point InitTime; //初始的时间
		std::deque<std::chrono::steady_clock::time_point> TimerContainer; //计时器 集合(Container)

		//是否保存所有的时间节点
		bool IsSaveAllTimePoint;

	public:
		/*
		* _IsSaveAllTimePoint: 是否保存所有的时间节点
		*	保存: 大量数据时, 占用内存; 接口传入(int)
		*	不保存: 只保存(第一次/前一次/后一次)的时间节点; 接口传入(TimePointLocation/tpl)
		*/
		Timer(bool _IsSaveAllTimePoint = false)
			: IsSaveAllTimePoint(_IsSaveAllTimePoint)
		{
			InitTime = this->GetTime();

			//不保存所有时间节点
			if (!this->IsSaveAllTimePoint) {
				this->TimerContainer.push_back(InitTime);
				this->TimerContainer.push_back(InitTime);
				this->TimerContainer.push_back(InitTime);
			}
			else {
				this->TimerContainer.push_back(InitTime);
			}
		}
	private:
		void Time_IsValid_RunTime(int _Number, const Tstr& _FunctionName)
		{
			if (this->IsSaveAllTimePoint) {
				_IsValid_RunTime<int>(_Number, [&](int _Number)->bool {
					if (_Number < 0 && _Number > this->TimerContainer.size() - 1) {
						return false;
					}
					return true;
					}, _LOGERRORINFO(_FunctionName + ": Location < 0 || Location > TimerContainer.size() - 1"));
			}
			else {
				_IsValid_RunTime<int>(_Number, [&](int _Number)->bool {
					if (_Number < 0 && _Number > this->TimerContainer.size() - 1) {
						return false;
					}
					return true;
					}, _LOGERRORINFO(_FunctionName + ": Location < 0 || Location > 2 (Location: 0/1/2)"));
			}
		}

	public:
		static std::chrono::steady_clock::time_point GetTime();

		void AddTimer();
		void AddTimer(const std::chrono::steady_clock::time_point& _TimePoint);
		void AddTimer(std::chrono::steady_clock::time_point&& _TimePoint);

		void SetTimer(const std::chrono::steady_clock::time_point& _TimePoint, int _Location);
		void SetTimer(std::chrono::steady_clock::time_point&& _TimePoint, int _Location);

		std::chrono::steady_clock::time_point GetTimer(int _Location);

		std::deque<std::chrono::steady_clock::time_point> GetTimerContainer();
		int GetTimerSize();

	public:
		template<class Target = sec>
		long long ComputTime_FirstToEnd()
		{
			// first - end
			return std::chrono::duration_cast<Target>(
				this->TimerContainer[this->TimerContainer.size() - 1] - this->TimerContainer[0]).count();
		}

		template<class Target = sec>
		long long ComputTime_FrontToBack()
		{
			// front - end
			return std::chrono::duration_cast<Target>(
				this->TimerContainer[this->TimerContainer.size() - 1] - this->TimerContainer[this->TimerContainer.size() - 2]).count();
		}
		template<class Target = sec>
		long long ComputTime(int _LocationBegin, int _LocationEnd)
		{
			Time_IsValid_RunTime(_LocationBegin, "ComputTime");
			Time_IsValid_RunTime(_LocationEnd, "ComputTime");

			// front - end
			return std::chrono::duration_cast<Target>(
				this->TimerContainer[LocationEnd] - this->TimerContainer[LocationBegin]).count();
		}

		// TimePoint: 时间
		// transformTarget: 转换前的时间计量
		// transformResult: 转换后的时间计量
		// return 转换后的时间
		template<class Target = sec, class Result = ms>
		static long long TransformTimes(const long long& TimePoint)
		{
			return std::chrono::duration_cast<Result>(Target(TimePoint)).count();
		}
	};

	//时间
	class Time {
	public:
		static bool IsShowLog;

	public:
		static void SetShowLog(bool _IsShowLog);

		template<class Target>
		static void sleep_s(long long _Number)
		{
			if (IsShowLog) {
				Terr << ANSIESC_YELLOW << _T("休眠: [") << _Number << _T("]") << TimeMeasureToString<Target>() << ANSIESC_RESET << endl;
			}
			std::this_thread::sleep_for(Target(_Number));
		}
		static void sleep(long long _sec)
		{
			if (IsShowLog) {
				Terr << ANSIESC_YELLOW << _T("休眠: [") << _sec << _T("]秒") << ANSIESC_RESET << endl;
			}
			std::this_thread::sleep_for(std::chrono::seconds(_sec));
		}
		template<class Target>
		static void wait_s(long long _Number)
		{
			if (IsShowLog) {
				Terr << ANSIESC_YELLOW << _T("等待: [") << _Number << _T("]") << TimeMeasureToString<Target>() << ANSIESC_RESET << endl;
			}
			Target timeTarget = std::chrono::duration_cast<Target>(std::chrono::steady_clock::now().time_since_epoch()) + Target(_Number);
			while (timeTarget > std::chrono::duration_cast<Target>(std::chrono::steady_clock::now().time_since_epoch())) {}
		}
		static void wait(long long _sec)
		{
			if (IsShowLog) {
				Terr << ANSIESC_YELLOW << _T("等待: [") << _sec << _T("]秒") << ANSIESC_RESET << endl;
			}
			std::chrono::seconds timeTarget = std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::steady_clock::now().time_since_epoch()) + std::chrono::seconds(_sec);
			while (timeTarget > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch())) {}
		}


		static void FormatTime(Tstr& text, const Tstr& timeFormat = _T("%Y-%m-%d %H:%M:%S"),
			const Tstr& textLeftFormat = _T("["), const Tstr& textRigthFormat = _T("]"))
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();;
			// 获取当前时间点（自epoch以来的时间）
			// 将时间点转换为time_t（用于localtime函数）
			std::time_t ttm = std::chrono::system_clock::to_time_t(now);
			// 使用localtime函数将time_t转换为本地时间（std::tm结构）
			std::tm* now_tm = std::localtime(&ttm);

			// 使用 std::put_time 格式化时间
			Tostringstream oss;
			oss << std::put_time(now_tm, timeFormat.c_str()); // 自定义时间格式

			//不需要修饰字符时, 直接返回格式化后的时间文本
			if (textLeftFormat == _T("") && textRigthFormat == _T("")) {
				text = oss.str() + text;
			}
			else {
				text = textLeftFormat + oss.str() + textRigthFormat + text;
			}
		}

		static Tstr GetFormatTime(const Tstr& timeFormat = _T("%Y-%m-%d %H:%M:%S"),
			const Tstr& textLeftFormat = _T("["), const Tstr& textRigthFormat = _T("]"))
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();;
			// 获取当前时间点（自epoch以来的时间）
			// 将时间点转换为time_t（用于localtime函数）
			std::time_t ttm = std::chrono::system_clock::to_time_t(now);
			// 使用localtime函数将time_t转换为本地时间（std::tm结构）
			std::tm* now_tm = std::localtime(&ttm);

			// 使用 std::put_time 格式化时间
			Tostringstream oss;
			oss << std::put_time(now_tm, timeFormat.c_str()); // 自定义时间格式

			//不需要修饰字符时, 直接返回格式化后的时间文本
			if (textLeftFormat == _T("") && textRigthFormat == _T("")) {
				return oss.str();
			}
			else {
				return textLeftFormat + oss.str() + textRigthFormat;
			}
		}
	};
}

#endif