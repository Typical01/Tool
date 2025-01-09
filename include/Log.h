#pragma once

#include "pch.h"
#include "Tchar_Typical.h"
#include "StringManage.h"
#include "Time_Typical.h"

#ifndef _LOG_H
#define _LOG_H


//#define _CONSOLE
#define _Thread
//#define _WCHAR
//#undef _WINDOWS


namespace Typical_Tool
{
	using namespace std;
	using namespace StringManage;


#ifdef _WINDOWS
	inline HWND hConsole; //控制台句柄
#endif

	enum LogMessage
	{
		tip, //提示: Tips [INFO] | 绿色
		war, //警告: Warning [WARRING] | 黄色
		err, //错误: Error [ERROR] | 红色
		lnf, //换行: '\n'
		end  //结束: 不换行
	};
	typedef LogMessage lm;
	LogMessage ts();
	LogMessage wr();
	LogMessage er();
	LogMessage ed();
	LogMessage lf();

	/* LogMessage = lm // 控制台颜色显示
	* 不需要颜色显示: #undef _ANSIESC_CONSOLE_CHAR_COLOR
	*/
	class Log {
	public:
		static LogMessage LastMessage;

	private:
		inline static Tofstream* LogFileStream_Out; //日志文件流 输出
		inline static mutex mutex_LogFileStream_Out;
		inline static queue<Tstr> LogFileWrite_Queue; //日志 WriteConfigFile队列
		inline static thread LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
		inline static atomic<bool> IsLogFileWriteThreadStop; //是否 停止日志文件写入线程

	private:
		static bool init; //初始化
		static bool IsLogFileWrite; //是否 写入日志文件
		static bool IsLogAllOutput; //是否 输出所有日志

		bool CMD; //控制台
		bool Release; //发行版

		bool ShowTime; //显示时间
		bool ShowLog; //显示日志
		bool SingleLogFile; //单一日志文件

	public:
		Log(bool cmd, bool release)
			: CMD(cmd), Release(release), ShowTime(true), ShowLog(true), SingleLogFile(true)
		{
			Init();
		}

		~Log();

	public:
		/* 控制台初始化状态: false(需要初始化), true(跳过初始化)
		*/
		template<class Temp = bool>
		void Init()
		{
			//控制台初始化
			if (!init) {
				// 设置全局区域为 "zh_CN.UTF-8"
				locale::global(locale("zh_CN.UTF-8"));

				if (!Tout.good()) {
					throw runtime_error(_LOGERRORINFO("Tout Not good!"));
				}
				if (!Terr.good()) {
					throw runtime_error(_LOGERRORINFO("Terr Not good!"));
				}

				// 使用Config进行格式化
				Tout.imbue(locale()); // 使用全局Config
				Terr.imbue(locale());

				//Tout << "Log Config: zh_CN.UTF-8 完成!\n";

#ifdef _WINDOWS
					//Windows 控制台编码修改: UTF-8

				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);

#ifndef _CONSOLE
				//分离控制台
				if (FreeConsole() == 0) {
					MessageBoxW(0, stow("log: 分离控制台失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + ToStr(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
				}
				else {
					if (!this->Release) {
						Terr << ANSIESC_GREEN << Log_ts << "Log: 分离控制台." << ANSIESC_RESET << Log_lf;
					}
				}


				//分配控制台: 当不是控制台程序时
				if (AllocConsole() == 0) {
					MessageBoxW(0, stow("log: 分配控制台失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + ToStr(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
				}
				else {
					if (!this->Release) {
						Terr << ANSIESC_GREEN << Log_ts << "Log: 分配控制台." << Log_lf;
					}
				}

				// 获取标准输出流的句柄
				FILE* FilePtr;
				// 重定向标准输出流
				freopen_s(&FilePtr, "CONOUT$", "w", stdout);
				if (FilePtr == nullptr) {
					MessageBoxW(0, stow("日志: 重定向标准输出流失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + to_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					return;
				}
				// 重定向标准错误流
				freopen_s(&FilePtr, "CONOUT$", "w", stderr);
				if (FilePtr == nullptr) {
					MessageBoxW(0, stow("日志: 重定向标准错误流失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + to_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					return;
				}

				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);
#endif

				// 获取控制台窗口的句柄
				hConsole = GetConsoleWindow();
#endif
				
				//完成初始化
				init = true;
			}
		}


	public:
		void LogWirte(const Tstr& _str)
		{
			if (IsLogFileWrite) { //写入文件
				lock_guard<mutex> QueueGuard(mutex_LogFileStream_Out);
				LogFileWrite_Queue.push(_str);
			}
		}


	private:
		void LogWirteToFile()
		{

			// 不退出, 且队列不为空
			while (1) {
				lock_guard<mutex> QueueGuard(mutex_LogFileStream_Out);

				//写入日志
				if (!LogFileWrite_Queue.empty()) {
					*LogFileStream_Out << LogFileWrite_Queue.front();
					LogFileWrite_Queue.pop();
				}
				else {
					if (IsLogFileWriteThreadStop.load()) { //停止写入线程
						if (LogFileStream_Out->good()) {
							LogFileStream_Out->close();
						}
						break;
					}
				}
			}

			if (!this->Release) {
				Terr << ANSIESC_YELLOW << Log_wr << _T("Log: 日志写入线程->退出!") << ANSIESC_RESET << Log_lf;
			}
		}

		void StopLogWrite()
		{
			if (!IsLogFileWriteThreadStop.load()) {
				IsLogFileWriteThreadStop.store(true); //退出 条件
				Time::wait_s<time::ms>(10); //主线程等待 后台退出
			}
		}

	private:

		void Log_Out(const Tstr& _ANSIESC_Color, Tostream& _ostream, Tstr& _str, const Tstr& _ANSIESC_RESET = ANSIESC_RESET, bool _IsWirteFile = false)
		{
			_ostream << _ANSIESC_Color; //在时间输出之前
			if (ShowTime) {
				_str = Time::GetFormatTime() + _str;
			}
			_ostream << _str;
			_ostream << _ANSIESC_RESET;

			//WriteConfigFile log日志
			if (IsLogAllOutput) { //所有级别
				LogWirte(_str);
			}
			else { //指定级别
				if (_IsWirteFile) {
					LogWirte(_str);
				}
			}
		}

		// _WINDOWS || _CONSOLE
		void Logs_ustr(const Tstr& text, const LogMessage& lm)
		{
			switch (lm) {
			case LogMessage::tip: {
				if (CMD) {
					Log_Out(ANSIESC_GREEN, Tout, Log_ts + text + Log_lf);
				}
				else {
#ifdef _WINDOWS
#ifndef _WCHAR
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_ts).c_str(), MB_OK);
#else
					MessageBoxW(NULL, text.c_str(), Log_ts, MB_OK);
#endif
#endif
				}

				break;
			}
			case LogMessage::war: {
				if (CMD) {
					Log_Out(ANSIESC_YELLOW, Tout, Log_wr + text + Log_lf);
				}
				else {
#ifdef _WINDOWS
#ifndef _WCHAR
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_wr).c_str(), MB_ICONWARNING);
#else
					MessageBoxW(NULL, text.c_str(), Log_wr, MB_ICONWARNING);
#endif
#endif
				}

				break;
			}
			case LogMessage::err: {
				if (CMD) {
					Log_Out(ANSIESC_RED, Terr, Log_er + text + Log_lf, ANSIESC_RESET, true);
				}
				else {
#ifdef _WINDOWS
#ifndef _WCHAR
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
#else
					MessageBoxW(NULL, text.c_str(), Log_er, MB_ICONSTOP);
#endif
#endif
				}

				break;
			}
			case LogMessage::end: {
				if (CMD) {
					Log_Out(_T(""), Terr, (Tstr)text, _T(""));

					return;
				}
				else {
#ifdef _WINDOWS
#ifndef _WCHAR
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_tx).c_str(), MB_ICONSTOP);
#else
					MessageBoxW(NULL, text.c_str(), Log_tx, MB_ICONSTOP);
#endif
#endif
				}

				break;
			}
			case LogMessage::lnf: {
				if (CMD) {
					Log_Out(_T(""), Terr, text + Log_lf, _T(""));

					return;
				}
				else {
#ifdef _WINDOWS
#ifndef _WCHAR
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_tx).c_str(), MB_ICONSTOP);
#else
					MessageBoxW(NULL, text.c_str(), Log_tx, MB_ICONSTOP);
#endif
#endif
				}

				break;
			}
			}
		}
		void Logs_lgm()
		{
			if (CMD) {
				Log_Out(_T(""), Tout, (Tstr)Log_lf, _T(""));
				
				return;
			}
		}


	public:

		//显示/隐藏 Log消息
		void operator()(const Tstr& text, LogMessage (*lm)() = &lf)
		{
			if (ShowLog) {
#ifdef _DEBUG
				Logs_ustr(text, lm());
				return;
#endif
				if (Release) {
					Logs_ustr(text, lm());
				}
			}
		}
		void operator()()
		{
			if (ShowLog) {
#ifdef _DEBUG
				Logs_lgm();
				return;
#endif
				if (Release) {
					Logs_lgm();
				}
			}
		}


	public:

		//设置日志显示
		void SetShowLog(bool showLog)
		{
			if (!this->Release) {
				Terr << ANSIESC_YELLOW << Log_wr << _T("Log 设置: 显示日志") << ANSIESC_RESET << Log_lf;
			}

			this->ShowLog = showLog;
		}
		//设置时间显示
		void SetShowTime(bool showTime)
		{
			if (!this->Release) {
				Terr << ANSIESC_YELLOW << Log_wr << _T("Log 设置: 显示时间") << ANSIESC_RESET << Log_lf;
			}

			this->ShowTime = showTime;
		}
		//设置是否为单一日志文件
		void SetSingleLogFile(bool _SingleLogFile)
		{
			if (!this->Release) {
				Terr << ANSIESC_YELLOW << Log_wr << _T("Log 设置: 单一日志文件: ") << std::boolalpha << _SingleLogFile << std::noboolalpha << ANSIESC_RESET << Log_lf;
			}

			this->SingleLogFile = _SingleLogFile;
		}
		//设置控制台显示: _WINDOWS(Win32 API)
		void SetShowConsole(bool showConsole)
		{
#ifdef _WINDOWS
			Terr << ANSIESC_YELLOW << Log_wr << _T("Log 设置 : 显示控制台") << ANSIESC_RESET << Log_lf;
			//显示/隐藏 窗口
			if (showConsole) {
				ShowWindow(hConsole, SW_SHOWDEFAULT);
			}
			else {
				ShowWindow(hConsole, SW_HIDE);
			}
#else
			if (!this->Release) {
				Terr << ANSIESC_YELLOW << Log_wr << _T("[Warring]Log 设置 : 显示控制台->无效(#ifndef _WINDOWS)") << ANSIESC_RESET << Log_lf;
			}
#endif
		}

	
	public:

		/* 设置所有日志写入
		* level:
		* er: Error log level output
		* !=3: All(lm: ts/wr/tx) log level output
		*/
		void SetAllLogFileWrite(bool logFileWrite, const Tstr& _LogFileName, int logLevel = lm::err)
		{
			IsLogFileWrite = logFileWrite;

			if (IsLogFileWrite) {
				//Terr << "Log 日志WriteConfigFile: 开始\n";

				//获取 当前路径/Log/Log文件名.txt 
				//创建文件夹 ./Log  .
				Tstr Log_FilePath = _LogFileName + _T("_Log.txt");
				if (!this->SingleLogFile) {
					Tstr Log_FolderName = (Tstr)_T(".") + PATH_SLASH + _T("Log");
					if (std::filesystem::exists(Log_FolderName)) { //目录存在
						if (std::filesystem::is_directory(Log_FolderName)) { // 是目录
							//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
							Tstr Log_FileName = Time::GetFormatTime(_T("%Y-%m-%d_%H-%M-%S_"), _T(""), _T(""));
							// ./Log/时间.txt  ||  ./时间.txt
							Tstr Log_FilePath = Log_FolderName + PATH_SLASH + Log_FileName + Log_FilePath;
						}
						else { // 不是目录
						}
					}
					else { //目录不存在
						std::filesystem::create_directory(Log_FolderName); //创建目录
						//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
						Tstr Log_FileName = Time::GetFormatTime(_T("%Y-%m-%d_%H-%M-%S_"), _T(""), _T(""));
						// ./Log/时间.txt  ||  ./时间.txt
						Tstr Log_FilePath = Log_FolderName + PATH_SLASH + Log_FileName + Log_FilePath;
					}
				}

				//打开文件
				if (!this->Release) {
					Terr << ANSIESC_GREEN << Log_ts << _T("Log: 日志输出文件名[") << Log_FilePath << _T("]") << ANSIESC_RESET << Log_lf;
				}
				LogFileStream_Out = new Tofstream(Log_FilePath, ios::out);
				if (!LogFileStream_Out->good()) {
					if (!this->Release) {
						Terr << ANSIESC_RED << Log_er << _T("Log: [") << Log_FilePath << _T("]打开文件失败!") << ANSIESC_RESET << Log_lf;
					}
					return;
				}
				else {
					if (!this->Release) {
						Tout << ANSIESC_GREEN << Log_ts << _T("Log: [") << Log_FilePath << _T("]打开文件成功!") << ANSIESC_RESET << Log_lf;
					}

					//Terr << "Log: 文件 开始写入!\n";
					//初始化: 日志WriteConfigFile线程
					std::thread Test(&Log::LogWirteToFile, this);
					LogFileProcessing = std::move(Test);
					LogFileProcessing.detach(); //分离线程
					if (!this->Release) {
						Terr << ANSIESC_GREEN << Log_ts << _T("Log: 日志写入线程 分离到后台.") << ANSIESC_RESET << Log_lf;
					}
				}
			}

			//输出所有级别
			if (logLevel != lm::err) {
				IsLogAllOutput = true;

				if (!this->Release) {
					Terr << ANSIESC_YELLOW << Log_wr
						<< _T("Log: File Output level >> All <<")
						<< ANSIESC_RESET << Log_lf;
				}
			}
			else {
				IsLogAllOutput = false;

				if (!this->Release) {
					Terr << ANSIESC_YELLOW << Log_wr
						<< _T("Log: File Output level >> Error <<")
						<< ANSIESC_RESET << Log_lf;
				}
			}
		}
	};

	/* 模式 mode : (_CONSOLE | _WINDOWS) && #ifdef _DEBUG
	* tip, war, Terr
	*/
	static std::shared_ptr<Log> LogAuto_Debug = std::make_shared<Log>(false, false);
	/* 模式 mode: (_CONSOLE | _WINDOWS) && #ifndef _DEBUG
	* tip, war, Terr
	*/
	static std::shared_ptr<Log> LogAuto_Release = std::make_shared<Log>(false, true);
	/* 模式 mode: (_CONSOLE) && #ifdef _DEBUG
	* tip, war, Terr
	*/
	static std::shared_ptr<Log> LogConsole_Debug = std::make_shared<Log>(true, false);
	/* 模式 mode: (_CONSOLE) && #ifndef _DEBUG
	* tip, war, Terr
	*/
	static std::shared_ptr<Log> LogConsole_Release = std::make_shared<Log>(true, true);

	// 包装为引用
	static std::reference_wrapper<Log> lg = *LogAuto_Debug;
	static std::reference_wrapper<Log> lgr = *LogAuto_Release;
	static std::reference_wrapper<Log> lgc = *LogConsole_Debug;
	static std::reference_wrapper<Log> lgcr = *LogConsole_Release;

	template<class T = bool>
	void Log_README()
	{
		Tout << _T("\t\tLog_README():")
			<< _T("#define _ANSIESC_CONSOLE_CHAR_COLOR: 控制台转义字符修改字符颜色\n\n")
			<< _T("#ifndef _DEBUG: _WINDOWS/_CONSOLE\n")
			<< _T("lgr(\"没有文件!\", wr); \n")
			<< _T("_DEBUG: _WINDOWS / _CONSOLE\n")
			<< _T("lg(\"关闭窗口\", ts); \n\n")
			<< _T("#ifndef _DEBUG: _CONSOLE\n")
			<< _T("lgcr(\"Code: 404\", er); \n")
			<< _T("_DEBUG: _CONSOLE\n")
			<< _T("lgc(\"Log.h\", lf); \n");
	}
}


#endif
