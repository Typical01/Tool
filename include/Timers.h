#pragma once

//Timers.h 计时器
#ifndef _TIMERS_H_
#define _TIMERS_H_

#include "pch.h"
//#include "Typical_Tool.h"
#include "Log.h"

namespace Typical_Tool {

	enum TimeMeasure
	{
		sec, //秒级
		ms, //毫秒级
		us, //微秒级
		ns  //纳秒级
	};
	typedef TimeMeasure tms;
#define 时间计量单位 TimeMeasure

	//计时器
	class  Timers
	{
	private:
		std::chrono::system_clock::time_point InitTime; //初始的时间
		std::chrono::steady_clock::time_point InitTime_s; //初始的时间
		std::chrono::system_clock::time_point TempTime; //临时的时间
		std::chrono::steady_clock::time_point TempTime_s; //临时的时间

		bool HighPrecision; //高精度
		TimeMeasure timeMeasure;

		std::vector<std::chrono::system_clock::time_point> TimerContainer; //计时器 集合(Container)
		std::vector<std::chrono::steady_clock::time_point> TimerContainer_s; //计时器 集合(Container)

	public:
		Timers(TimeMeasure tms = tms::sec)
		{
			if (tms != tms::sec) { //高精度
				InitTime_s = GetTime_s();
				TimerContainer_s.push_back(InitTime_s);
				HighPrecision = true;
				this->timeMeasure = tms;
			}
			else {
				InitTime = GetTime();
				TimerContainer.push_back(InitTime);
				HighPrecision = false;
				this->timeMeasure = tms;
			}
		}

	public:
		static std::chrono::system_clock::time_point GetTime();
#define 获取时间 GetTime
	
		void AddTimer();
		void AddTimer(const std::chrono::system_clock::time_point& time);
		void AddTimer(std::chrono::system_clock::time_point&& time);
#define 添加时间戳 AddTimer

		void SetTimer(const std::chrono::system_clock::time_point& time, int Location);
		void SetTimer(std::chrono::system_clock::time_point&& time, int Location);
#define 设置时间戳 SetTimer
		
		std::chrono::system_clock::time_point GetTimer(int Location);
#define 获取时间戳 GetTimer

	public:
		std::chrono::steady_clock::time_point GetTime_s();
#define 获取时间_高精度 GetTime_s

		void AddTimer_s();
		void AddTimer_s(const std::chrono::steady_clock::time_point& time);
		void AddTimer_s(std::chrono::steady_clock::time_point&& time);
#define 添加时间戳_高精度 AddTimer_s

		void SetTimer_s(const std::chrono::steady_clock::time_point& time, int Location);
		void SetTimer_s(std::chrono::steady_clock::time_point&& time, int Location);
#define 设置时间戳_高精度 SetTimer_s

		std::chrono::steady_clock::time_point GetTimer_s(int Location);
#define 获取时间戳_高精度 GetTimer_s
	
	public:
		long long ComputationFirstToEndInterval();
		long long ComputationFrontToBackInterval();
		long long ComputationInterval(int LocationBegin, int LocationEnd);
#define 计算前后间隔 ComputationFrontToBackInterval
#define 计算首尾间隔 ComputationFirstToEndInterval
#define 计算间隔 ComputationInterval

		// tms: 时间计量 / time: 时间 / transformLaterTms: 转换后的时间计量
		// return 转换后的时间
		static long long TransformTimes(const long long& time, bool& isSucceed, TimeMeasure tms = tms::sec, TimeMeasure transformLaterTms = tms::ms);
		static bool TransformTimes(long long& time, TimeMeasure tms = tms::sec, TimeMeasure transformLaterTms = tms::ms);
#define 时间转换 TransformTime
		
		static void sleep_s(long long ms);
		template<class T = bool>
		static void sleep(long long sec)
		{
			for (long long temp = 1; temp < sec; temp++) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				lgc("暂停: " + Uto_string(temp) + "秒", lgm::ts);
			}
		}
#define 延迟_毫秒 sleep_s
#define 延迟_秒 sleep

	public:
		std::vector<std::chrono::system_clock::time_point> GetTimerContainer();
#define 获取计时器容器 GetTimerContainer
		std::vector<std::chrono::steady_clock::time_point> GetTimerContainer_s();
#define 获取计时器容器_高精度 GetTimerContainer_s
		int GetTimerSize();
#define 获取计时器大小 GetTimerSize
		int GetTimerSize_s();
#define 获取计时器大小_高精度 GetTimerSize_s

		// [time]text
		template<class Temp = bool>
		static void FormattingTime(Ustr& text, const Ustr& timeFormat = "%Y-%m-%d %H:%M:%S", 
			const Ustr& textLeftFormat = "[", const Ustr& textRigthFormat = "]")
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();;
			// 获取当前时间点（自epoch以来的时间）
			// 将时间点转换为time_t（用于localtime函数）
			std::time_t tm = std::chrono::system_clock::to_time_t(now);
			// 使用localtime函数将time_t转换为本地时间（std::tm结构）
			std::tm* now_tm = std::localtime(&tm);

			// 使用 std::put_time 格式化时间
			Uostringstream oss;
			oss << std::put_time(now_tm, timeFormat.c_str()); // 自定义时间格式

			//不需要修饰字符时, 直接返回格式化后的时间文本
			if (textLeftFormat == "" && textRigthFormat == "") {
				text = oss.str() + text;
			}
			text = textLeftFormat + oss.str() + textRigthFormat + text;
		}
		// [time]text
		template<class Temp = bool>
		static Ustr GetFormattingTime(const Ustr& timeFormat = "%Y-%m-%d %H:%M:%S", 
			const Ustr& textLeftFormat = "[", const Ustr& textRigthFormat = "]")
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();;
			// 获取当前时间点（自epoch以来的时间）
			// 将时间点转换为time_t（用于localtime函数）
			std::time_t tm = std::chrono::system_clock::to_time_t(now);
			// 使用localtime函数将time_t转换为本地时间（std::tm结构）
			std::tm* now_tm = std::localtime(&tm);

			// 使用 std::put_time 格式化时间
			Uostringstream oss;
			oss << std::put_time(now_tm, timeFormat.c_str()); // 自定义时间格式

			//不需要修饰字符时, 直接返回格式化后的时间文本
			if (textLeftFormat == "" && textRigthFormat == "") {
				return oss.str();
			}
			return textLeftFormat + oss.str() + textRigthFormat;
		}
#define 格式化时间 FormattingTime;
#define 获取格式化时间 GetFormattingTime;

	};
}

#endif