#pragma once

//Timer.h 计时器
#ifndef _TIMERS_H_
#define _TIMERS_H_

#include <libTypical/Tool/pch.h>
#include <libTypical/Tool/Tchar_Typical.h>
#include <libTypical/Tool/Log.h>



namespace Typical_Tool {
	namespace time {
		using century = std::chrono::duration<long long, std::ratio<60LL * 60LL * 24LL * 7LL * 30LL * 12LL * 100LL, 1LL>>;	// 世纪
		using year = std::chrono::duration<long long, std::ratio<60LL * 60LL * 24LL * 7LL * 30LL * 12LL, 1LL>>;				// 年
		using month = std::chrono::duration<long long, std::ratio<60LL * 60LL * 24LL * 7LL * 30LL, 1LL>>;					// 月
		using week = std::chrono::duration<long long, std::ratio<60LL * 60LL * 24LL * 7LL, 1LL>>;							// 周
		using day = std::chrono::duration<long long, std::ratio<60LL * 60LL * 24LL, 1LL>>;									// 天
		using hour = std::chrono::duration<long long, std::ratio<60LL * 60LL, 1LL>>;										// 时
		using min = std::chrono::duration<long long, std::ratio<60LL, 1LL>>;												// 分
		using sec = std::chrono::seconds;																					// 秒
		using ms = std::chrono::milliseconds;																				// 毫秒
		using us = std::chrono::microseconds;																				// 微秒
		using ns = std::chrono::nanoseconds;																				// 纳秒
	}

	enum TimeMeasure
	{
		ns,		// 纳秒
		us,		// 微秒
		ms,		// 毫秒
		sec,	// 秒
		min,	// 分
		hour,	// 时
		day,	// 天
		week,	// 周
		month,	// 月
		year,	// 年
		century	// 世纪
	};
	using Tm = TimeMeasure;

	template<typename Target = bool>
	Tstr TimeMeasureToString(TimeMeasure _Tm) {
		switch (_Tm) {
		case TimeMeasure::ns: {
			return TEXT("ns");
		}
		case TimeMeasure::us: {
			return TEXT("us");
		}
		case TimeMeasure::ms: {
			return TEXT("ms");
		}
		case TimeMeasure::sec: {
			return TEXT("s");
		}
		case TimeMeasure::min: {
			return TEXT("m");
		}
		case TimeMeasure::hour: {
			return TEXT("h");
		}
		case TimeMeasure::day: {
			return TEXT("day");
		}
		case TimeMeasure::week: {
			return TEXT("week");
		}
		case TimeMeasure::month: {
			return TEXT("month");
		}
		case TimeMeasure::year: {
			return TEXT("year");
		}
		case TimeMeasure::century: {
			return TEXT("century");
		}
		default: {
			return TEXT("unknown");
		}
		}
	}

	template<typename Target>
	Tstr TimeMeasureToString() {
		if constexpr (std::is_same<Target, time::century>::value) {
			return TEXT("century");
		}
		if constexpr (std::is_same<Target, time::year>::value) {
			return TEXT("year");
		}
		if constexpr (std::is_same<Target, time::month>::value) {
			return TEXT("month");
		}
		if constexpr (std::is_same<Target, time::week>::value) {
			return TEXT("week");
		}
		if constexpr (std::is_same<Target, time::day>::value) {
			return TEXT("day");
		}
		else if constexpr (std::is_same<Target, time::hour>::value) {
			return TEXT("h");
		}
		else if constexpr (std::is_same<Target, time::min>::value) {
			return TEXT("m");
		}
		else if constexpr (std::is_same<Target, time::sec>::value) {
			return TEXT("s");
		}
		else if constexpr (std::is_same<Target, time::ms>::value) {
			return TEXT("ms");
		}
		else if constexpr (std::is_same<Target, time::us>::value) {
			return TEXT("us");
		}
		else if constexpr (std::is_same<Target, time::ns>::value) {
			return TEXT("ns");
		}
		else {
			return TEXT("unknown");
		}
	}
#define TimeToStr TimeMeasureToString

	/// <summary>
	/// 时间转换
	/// </summary>
	/// <param name="Target">: 转换前的时间计量</param>
	/// <param name="Result">: 转换后的时间计量</param>
	/// <param name="TimePoint">: 时间</param>
	/// <returns></returns>
	template<class Target = time::sec, class Result = time::ms>
	static long long TransformTimes(const long long& TimePoint)
	{
		return std::chrono::duration_cast<Result>(Target(TimePoint)).count();
	}

	/// <summary>
	/// 时间单位自动转换字符串
	/// </summary>
	/// <param name="Target">: NULL</param>
	/// <param name="_Time">: 当前时间</param>
	/// <param name="_Tm">: 当前时间的单位</param>
	/// <param name="_bShowMeasure">: 字符串结果带单位</param>
	/// <param name="_FormatPlaceholder">: 格式占位符宽度, 进位不足时补 '0'</param>
	/// <param name="_TimeIntervalStr">: 每级单位时间的间隔符号</param>
	/// <param name="_NumberLeftIntervalStr">: 突出当前时间的格式 Left</param>
	/// <param name="_NumberRightIntervalStr">: 突出当前时间的格式 Right</param>
	/// <returns></returns>
	template <class Target = bool>
	Tstr TimeAutoToStr(long long _Time, TimeMeasure _Tm, bool _bShowMeasure = true, int _FormatPlaceholder = 1, const Tstr& _TimeIntervalStr = TEXT(" "),
		const Tstr& _NumberLeftIntervalStr = TEXT(""), const Tstr& _NumberRightIntervalStr = TEXT("")) {
		if (_Tm == TimeMeasure::century) {
			if (_bShowMeasure) {
				return ToStr(_Time) + _TimeIntervalStr + TimeToStr(Tm::century);
			}
			else {
				return ToStr(_Time) + _TimeIntervalStr;
			}
		}

		Tstr Result;
		std::map<TimeMeasure, long long> TimeStr; // 存储各时间单位对应的值
		std::vector<long long> Measures = { 1000, 1000, 1000, 60, 60, 24, 7, 4, 12, 100 }; // 转换关系

		// 从当前单位向更高级别单位转换
		for (int i = _Tm; i < Measures.size(); ++i) {
			if (_Time >= Measures[i]) {
				auto NowMeasure = _Time % Measures[i]; // 当前级别的余数
				auto HigherMeasure = _Time / Measures[i]; // 升级后的值

				TimeStr[static_cast<TimeMeasure>(i)] = NowMeasure; // 存储当前级别的值
				_Time = HigherMeasure; // 更新为更高级别的值
			}
			else {
				TimeStr[static_cast<TimeMeasure>(i)] = _Time; // 存储剩余值
				break; // 无需继续转换
			}
		}

		// 使用 std::ostringstream 进行格式化输出
		Tostringstream oss;

		// 将 map 中的结果拼接成字符串
		for (auto it = TimeStr.rbegin(); it != TimeStr.rend(); ++it) { // 从高单位到低单位拼接
			if (!Result.empty()) {
				Result += _TimeIntervalStr; // 添加间隔符
			}
			if (_bShowMeasure) {
				// 格式化数字，保证数字至少为两位
				oss.str(TEXT(""));  // 清空字符串流
				oss.clear();  // 清除错误标志
				oss << std::setw(_FormatPlaceholder) << std::setfill(TEXT('0')) << it->second;  // 设置最小宽度为 2，填充字符为 0
				Result += _NumberLeftIntervalStr + oss.str() + _NumberRightIntervalStr + TimeToStr(it->first); // 拼接单位
			}
			else {
				// 格式化数字，保证数字至少为两位
				oss.str(TEXT(""));  // 清空字符串流
				oss.clear();  // 清除错误标志
				oss << std::setw(_FormatPlaceholder) << std::setfill(TEXT('0')) << it->second;  // 设置最小宽度为 2，填充字符为 0
				Result += _NumberLeftIntervalStr + oss.str() + _NumberRightIntervalStr; // 拼接间隔
			}
		}

		return Result;
	}

	enum class TimePointLocation {
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
		std::vector<std::chrono::steady_clock::time_point> TimerContainer; //计时器 集合(Container)
		std::mutex mutex_Timer;

		//是否保存所有的时间节点
		bool IsSaveAllTimePoint;

	public:
		/// <summary>
		/// 保存: 大量数据时, 占用内存; 接口传入(int) |
		/// 不保存: 只保存(第一次/前一次/后一次)的时间节点; 接口传入(TimePointLocation/tpl)
		/// </summary>
		/// <param name="_IsSaveAllTimePoint">: 是否保存所有的时间节点</param>
		Timer(bool _IsSaveAllTimePoint = false)
			: IsSaveAllTimePoint(_IsSaveAllTimePoint)
		{
			auto InitTime = this->GetTime();

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
					}, _LOGERRORINFO(_FunctionName + TEXT(": Location < 0 || Location > TimerContainer.size() - 1")));
			}
			else {
				_IsValid_RunTime<int>(_Number, [&](int _Number)->bool {
					if (_Number < 0 && _Number > this->TimerContainer.size() - 1) {
						return false;
					}
					return true;
					}, _LOGERRORINFO(_FunctionName + TEXT(": Location < 0 || Location > 2 (Location: 0/1/2)")));
			}
		}

	public:
		static std::chrono::steady_clock::time_point GetTime();

		void AddTimer();
		void AddTimer(const std::chrono::steady_clock::time_point& _TimePoint);
		void AddTimer(std::chrono::steady_clock::time_point&& _TimePoint);

		void SetTimer(const std::chrono::steady_clock::time_point& _TimePoint, int _Location) {
			Time_IsValid_RunTime(_Location, TEXT("SetTimer()"));

			std::lock_guard<std::mutex> tempMutex(this->mutex_Timer);
			this->TimerContainer[_Location] = _TimePoint;
		}
		void SetTimer(std::chrono::steady_clock::time_point&& _TimePoint, int _Location) {
			Time_IsValid_RunTime(_Location, TEXT("SetTimer()"));

			std::lock_guard<std::mutex> tempMutex(this->mutex_Timer);
			this->TimerContainer[_Location] = _TimePoint;
		}
		std::chrono::steady_clock::time_point GetTimer(int _Location) {
			Time_IsValid_RunTime(_Location, TEXT("GetTimer()"));

			std::lock_guard<std::mutex> tempMutex(this->mutex_Timer);
			return this->TimerContainer[_Location];
		}

		std::vector<std::chrono::steady_clock::time_point> GetTimerContainer();
		int GetTimerSize();

	public:
		template<class Target = time::sec>
		long long ComputTime_FirstToEnd()
		{
			// first - end
			return std::chrono::duration_cast<Target>(
				this->TimerContainer[this->TimerContainer.size() - 1] - this->TimerContainer[0]).count();
		}

		template<class Target = time::sec>
		long long ComputTime_FrontToBack()
		{
			// front - end
			return std::chrono::duration_cast<Target>(
				this->TimerContainer[this->TimerContainer.size() - 1] - this->TimerContainer[this->TimerContainer.size() - 2]).count();
		}
		template<class Target = time::sec>
		long long ComputTime(int _LocationBegin, int _LocationEnd)
		{
			Time_IsValid_RunTime(_LocationBegin, TEXT("ComputTime"));
			Time_IsValid_RunTime(_LocationEnd, TEXT("ComputTime"));

			// front - end
			return std::chrono::duration_cast<Target>(
				this->TimerContainer[_LocationEnd] - this->TimerContainer[_LocationBegin]).count();
		}
	};



	//时间
	class Time {
	public:
		static bool IsShowLog;

	public:
		static void SetShowLog(bool _IsShowLog);

		template<class Target = time::sec>
		static void sleep(long long _Number)
		{
			if (IsShowLog) {
				LogDebug(War, Printf(TEXT("休眠: [%s]%s"), _Number, TimeMeasureToString<Target>()));
			}
			std::this_thread::sleep_for(Target(_Number));
		}
		template<class Target = time::sec>
		static void wait(long long _Number)
		{
			if (IsShowLog) {
				LogDebug(War, Printf(TEXT("等待: [%s]%s"), _Number, TimeMeasureToString<Target>()));
			}
			Target timeTarget = std::chrono::duration_cast<Target>(std::chrono::steady_clock::now().time_since_epoch()) + Target(_Number);
			while (timeTarget > std::chrono::duration_cast<Target>(std::chrono::steady_clock::now().time_since_epoch())) {}
		}


		static void FormatTime(Tstr& text, const Tstr& timeFormat = TEXT("%Y-%m-%d %H:%M:%S"),
			const Tstr& textLeftFormat = TEXT("["), const Tstr& textRigthFormat = TEXT("]"))
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
			if (textLeftFormat == TEXT("") && textRigthFormat == TEXT("")) {
				text = oss.str() + text;
			}
			else {
				text = textLeftFormat + oss.str() + textRigthFormat + text;
			}
		}

		static Tstr GetFormatTime(const Tstr& timeFormat = TEXT("%Y-%m-%d %H:%M:%S"),
			const Tstr& textLeftFormat = TEXT("["), const Tstr& textRigthFormat = TEXT("]"))
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
			if (textLeftFormat == TEXT("") && textRigthFormat == TEXT("")) {
				return oss.str();
			}
			else {
				return textLeftFormat + oss.str() + textRigthFormat;
			}
		}
	};
}

#endif