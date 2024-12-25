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


	static bool IsLogFileWrite; //是否 写入日志文件
	static bool IsLogAllOutput; //是否 输出所有日志
#ifdef _Thread
	static Tofstream LogFileStream_Out; //日志文件流 输出
	static mutex mutex_LogFileStream_Out;
	static queue<Tstr> IsLogFileWrite_Queue; //日志 WriteConfigFile队列
	static thread LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
	static atomic<bool> IsLogFileWriteThreadStop; //是否 停止日志文件写入线程
	static condition_variable cv_LogFileQueue; //通知 日志文件队列
#endif


#ifdef _WINDOWS
	static HWND hConsole; //控制台句柄
#endif


	enum LogMessage
	{
		tips, //提示: Tips [INFO] | 绿色
		war, //警告: Warning [WARRING] | 黄色
		err, //错误: Error [ERROR] | 红色
		txt, //文本
		lnf, //换行: '\n' | 还原 ANSIESC颜色
		end  //结束: 还原 ANSIESC颜色
	};
	typedef LogMessage lm;
	LogMessage ts();
	LogMessage wr();
	LogMessage er();
	LogMessage ed();
	LogMessage lf();


	class Log {
	public:
		static LogMessage LastMessage;
	private:
		static bool init; //初始化

		bool CMD; //控制台
		bool Release; //发行版

		bool ShowTime; //显示时间
		bool ShowLog; //显示日志

	public:
		Tostream& Tout = temp_out;
		Tostream& Terr = temp_err;

	public:
		Log(bool cmd, bool release)
			: CMD(cmd), Release(release), ShowTime(true), ShowLog(true)
		{
			Init();
		}


		template<class T = bool>
		Log& operator<<(LogMessage(*_lm)())
		{
			//输出模式: out || err 
			switch (_lm()) {
			case LogMessage::tips: {
				if (Log::LastMessage == lm::end) {
					Log::LastMessage = lm::tips;
					this->Tout << ANSIESC_GREEN << Log_ts;
				}

				break;
			}
			case LogMessage::war: {
				if (Log::LastMessage == lm::end) {
					Log::LastMessage = lm::war;
					this->Terr << ANSIESC_YELLOW << Log_wr;
				}

				break;
			}
			case LogMessage::err: {
				if (Log::LastMessage == lm::end) {
					Log::LastMessage = lm::err;
					this->Terr << ANSIESC_RED << Log_er;
				}

				break;
			}
			case LogMessage::end: {
				if (Log::LastMessage != lm::end && Log::LastMessage != lm::lnf) {
					Log::LastMessage = lm::end;
					this->Tout << ANSIESC_RESET; //还原颜色
				}

				break;
			}
			case LogMessage::lnf: {
				if (Log::LastMessage != lm::end) {
					if (Log::LastMessage == lm::lnf) {
						Log::LastMessage = lm::lnf;
						this->Tout << _T('\n');
					}
					else {
						Log::LastMessage = lm::lnf;
						this->Tout << _T('\n') << ANSIESC_RESET; //还原颜色
					}
				}

				break;
			}
			}

			return this;
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
					MessageBoxW(0, stow("错误代码: " + To_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
				}
				else {
					Tout << "Log: 分离控制台\n";
				}


				//分配控制台: 当不是控制台程序时
				if (AllocConsole() == 0) {
					MessageBoxW(0, stow("log: 分配控制台失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + To_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
				}
				else {
					Tout << "Log: 分配控制台\n";
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
				if (IsLogFileWrite) {
					//Terr << "Log 日志WriteConfigFile: 开始\n";

					//获取 当前路径/Log/Log文件名.txt 
					//创建文件夹 ./Log  .
					Tstr Log_FolderName = (Tstr)_T(".") + PATH_BACKSLASH + _T("Log");
					if (CreateFolder(Log_FolderName)) {
					}
					else {
						Terr << _T("Log: 文件夹[" + Log_FolderName + "]创建失败!\n");

						Log_FolderName = (Tstr)".";
					}

					//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
					Tstr Log_FileName = Time::GetFormatTime(_T("%Y-%m-%d_%H-%M-%S"), _T(""), _T("")) + _T(".txt");
					// ./Log/时间.txt  ||  ./时间.txt
					Tstr Log_FilePath = Log_FolderName + PATH_BACKSLASH + Log_FileName; //总是添加 /

					//打开文件
					Terr << "Log: 日志输出文件名 " << Log_FilePath << "\n";
					LogFileStream_Out = move(ofstream(Log_FilePath, ios::out));
					if (!LogFileStream_Out) {
						Terr << _T("Log: " + Log_FilePath + " 打开文件失败!\n");
						return;
					}
					Terr << _T("Log: " + Log_FilePath + " 打开文件成功!\n");
					//Terr << "Log: 文件 开始写入!\n";
#ifdef _Thread
					//初始化: 日志WriteConfigFile线程
					LogFileProcessing = thread(&Log::LogWirteToFile,
						this);
					LogFileProcessing.detach(); //分离线程
					Terr << _T("Log: 日志写入线程 分离到后台\n");
#endif
				}

				//完成初始化
				init = true;
			}
		}


	public:
		void LogWirte(const Tstr& _str)
		{
			if (IsLogFileWrite) {
#ifdef _Thread
				{
					//锁 日志WriteConfigFile队列
					if (IsLogAllOutput) {
						lock_guard<mutex> lock(mutex_LogFileStream_Out); // 上锁
						IsLogFileWrite_Queue.push(_str);
					}
				}
				cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
				if (IsLogAllOutput) {
					if (ShowTime) {
						LogFileStream_Out << Time::GetFormatTime();
					}
					LogFileStream_Out << _str;
				}
			}
		}


	private:
		void LogWirteToFile()
		{
#ifdef _Thread
			//不是停止线程时
			while (!IsLogFileWriteThreadStop.load()) {
				//锁 日志文件流
				unique_lock<mutex> queueLock(mutex_LogFileStream_Out); // 上锁
				cv_LogFileQueue.wait(queueLock, [this] {
					return !IsLogFileWrite_Queue.empty() || IsLogFileWriteThreadStop.load();
					});

				if (IsLogFileWriteThreadStop.load() && IsLogFileWrite_Queue.empty()) {
					break; // 退出循环
				}

				//日志队列非空
				if (!IsLogFileWrite_Queue.empty()) {
					LogFileStream_Out << IsLogFileWrite_Queue.front();
					IsLogFileWrite_Queue.pop();
				}
				else { //日志为空: 等待 100ms
					queueLock.unlock();
					this_thread::sleep_for(chrono::milliseconds(100));
				}

			}
#endif
		}

		template<class T = bool>
		void StopLogWrite()
		{
#ifdef _Thread
			IsLogFileWriteThreadStop.store(true);
			cv_LogFileQueue.notify_one(); // 通知线程退出
			Terr << _T("Log: 日志写入线程 退出\n");
#endif
		}

	private:

		template<class T = bool>
		void SetConsoleTextColor_Error(const Tstr& _ANSIESC)
		{
			Terr << _ANSIESC;
		}
		template<class T = bool>
		void ConsoleOutput_Error(const Tstr& _text)
		{
			Terr << _text;
		}
		template<class T = bool>
		void ReSetConsoleTextColor_Error()
		{
			Terr << ANSIESC_RESET;
		}

		template<class T = bool>
		void SetConsoleTextColor(const Tstr& _ANSIESC)
		{
			Tout << _ANSIESC;
		}
		template<class T = bool>
		void ConsoleOutput(const Tstr& _text)
		{
			Tout << _text;
		}
		template<class T = bool>
		void ReSetConsoleTextColor()
		{
			Tout << ANSIESC_RESET;
		}

		// _WINDOWS || _CONSOLE
		template<class T = bool>
		void Logs_ustr(const Tstr& text, const LogMessage& lm)
		{
			switch (lm) {
			case LogMessage::tips: {
				if (CMD) {
					Tstr temp;
					SetConsoleTextColor(ANSIESC_GREEN); //在时间输出之前
					if (ShowTime) {
						Tout << Time::GetFormatTime();
					}
					temp = (Tstr)Log_ts + text;
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//WriteConfigFile log日志
					LogWirte(temp);
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
					Tstr temp;
					SetConsoleTextColor(ANSIESC_YELLOW); //在时间输出之前
					if (ShowTime) {
						Tout << Time::GetFormatTime();
					}
					temp = (Tstr)Log_wr + text + "\n";
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//WriteConfigFile log日志
					LogWirte(temp);
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
					Tstr temp;
					SetConsoleTextColor_Error(ANSIESC_RED); //在时间输出之前
					if (ShowTime) {
						Terr << Time::GetFormatTime();
					}
					temp = (Tstr)Log_er + text + "\n";
					ConsoleOutput_Error(temp);
					ReSetConsoleTextColor_Error();

					//WriteConfigFile log日志
					LogWirte(temp);
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
			case LogMessage::txt: {
				if (CMD) {
					if (ShowTime) {
						Tout << Time::GetFormatTime();
					}
					Tstr temp = text + '\n';
					ConsoleOutput(temp);

					//WriteConfigFile log日志
					LogWirte(temp);
					return;
				}
			}
			}
		}
		template<class T = bool>
		void Logs_lgm()
		{
			if (CMD) {
				if (ShowTime) {
					Tout << Time::GetFormatTime();
				}
				Tstr temp = "\n";
				ConsoleOutput(temp);

				//WriteConfigFile log日志
				LogWirte(temp);
				return;
			}
		}


		// _DEBUG || NDEBUG
		template<class T = bool>
		void Logs()
		{
#ifdef _DEBUG
			Logs_lgm();
			return;
#endif
			if (Release) {
				Logs_lgm();
			}
		}

	public:

		//显示/隐藏 Log消息
		template<class T = bool>
		void operator()(const Tstr& text, const LogMessage& lm = lm::txt)
		{
			if (ShowLog) {
				Logs_ustr(text, lm);
			}
		}
		template<class T = bool>
		void operator()(Tstr&& text, const LogMessage& lm = lm::txt)
		{
			if (ShowLog) {
				Logs_ustr(text, lm);
			}
		}
		template<class T = bool>
		void operator()()
		{
			if (ShowLog) {
				Logs();
			}
		}


	private:

		// _WINDOWS: Win32 API
		template<class T = bool>
		bool CreateFolder(const Tstr& folderPath)
		{
#ifdef _WINDOWS
			DWORD attributes = GetFileAttributes(folderPath.c_str());

			// 检查路径是否存在且不是目录  
			if (attributes == INVALID_FILE_ATTRIBUTES) {
				// 路径不存在或出错，尝试创建目录  
				if (CreateDirectory(folderPath.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
					Tout << _T("Log: 文件夹[" + folderPath + "]创建成功!") + "\n";
					// 创建成功
					return true;
				}
				Terr << _T("Log: 文件夹[" + folderPath + "]创建失败!") + "\n";
				// 创建失败且不是因为路径已存在  
				return false;
			}
			else if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
				Terr << _T("Log: 文件夹[" + folderPath + "]已存在") + "\n";
				// 路径已经是一个目录  
				return true;
			}
			// 路径存在但不是目录（可能是一个文件）  
			Terr << _T("Log: 文件夹[" + folderPath + "]创建失败(路径存在, 但不是目录)!") + "\n";
#else
			Terr << _T("Log: 文件夹[" + folderPath + "]创建失败(#ifndef _WINDOWS)!") + "\n";
#endif
			return false;
		}


	public:

		//设置日志显示
		template<class Temp = bool>
		void SetShowLog(bool showLog)
		{
			Terr << _T("Log 设置: 显示日志\n") << std::endl;

			this->ShowLog = showLog;
		}
		//设置时间显示
		template<class Temp = bool>
		void SetShowTime(bool showTime)
		{
			Terr << _T("Log 设置: 显示时间\n") << std::endl;

			this->ShowTime = showTime;
		}
		//设置控制台显示: _WINDOWS(Win32 API)
		template<class Temp = bool>
		void SetShowConsole(bool showConsole)
		{
#ifdef _WINDOWS
			Terr << _T("Log 设置 : 显示控制台\n") << std::endl;
			//显示/隐藏 窗口
			if (showConsole) {
				ShowWindow(hConsole, SW_SHOWDEFAULT);
			}
			else {
				ShowWindow(hConsole, SW_HIDE);
			}
#else
			Terr << _T("[Warring]Log 设置 : 显示控制台 无效\n") << std::endl;
#endif
		}

	
	public:

		/* 设置所有日志写入
		* level:
		* er: Error log level output
		* !=3: All(lm: ts/wr/tx) log level output
		*/
		template<class T = bool>
		static void SetAllIsLogFileWrite(bool logFileWrite, int logLevel = lm::err)
		{
			IsLogFileWrite = logFileWrite;

			//输出所有级别
			if (logLevel != lm::err) {
				IsLogAllOutput = true;

				Tout << ANSIESC_YELLOW
					<< _T("Log: File Output level >> All <<")
					<< ANSIESC_RESET << endl;
			}
			else {
				IsLogAllOutput = false;

				Tout << ANSIESC_YELLOW
					<< _T("Log: File Output level >> Error <<")
					<< ANSIESC_RESET << endl;
			}
		}
	};

	/* 模式 mode : (_CONSOLE | _WINDOWS) && #ifdef _DEBUG
	* tip, war, Terr
	*/
	static Log lg(false, false);
	/* 模式 mode: (_CONSOLE) && #ifdef _DEBUG
	* tip, war, Terr
	*/
	static Log lgc(true, false);
	/* 模式 mode: (_CONSOLE | _WINDOWS) && #ifndef _DEBUG
	* tip, war, Terr
	*/
	static Log lgr(false, true);
	/* 模式 mode: (_CONSOLE) && #ifndef _DEBUG
	* tip, war, Terr
	*/
	static Log lgcr(true, true);

	
	template<class T>
	Log& operator<<(Log& _Log, T _To_Var)
	{
		Tstr temp = To_string(_To_Var);
		if (Log::LastMessage == lm::err) {
			_Log.Terr << temp;
		}
		else {
			_Log.Tout << temp;
		}

		_Log.LogWirte(temp);
	}
	template<class T>
	Log& operator<<(Log& _Log, const T& _str)
	{
		if (Log::LastMessage == lm::err) {
			_Log.Terr << _str;
		}
		else {
			_Log.Tout << _str;
		}

		_Log.LogWirte(_str);
	}
	template<class T = bool>
	Log& operator<<(Log& _Tostream, LogMessage(*_lm)())
	{
		//输出模式: Tout || Terr
		switch (_lm()) {
		case LogMessage::tips: {
			if (Log::LastMessage == lm::end) {
				Log::LastMessage = lm::tips;
				_Tostream.Tout << ANSIESC_GREEN << Log_ts;
			}

			break;
		}
		case LogMessage::war: {
			if (Log::LastMessage == lm::end) {
				Log::LastMessage = lm::war;
				_Tostream.Terr << ANSIESC_YELLOW << Log_wr;
			}

			break;
		}
		case LogMessage::err: {
			if (Log::LastMessage == lm::end) {
				Log::LastMessage = lm::err;
				_Tostream.Terr << ANSIESC_RED << Log_er;
			}

			break;
		}
		case LogMessage::end: {
			if (Log::LastMessage != lm::end && Log::LastMessage != lm::lnf) {
				Log::LastMessage = lm::end;
				_Tostream.Tout << ANSIESC_RESET; //还原颜色
			}

			break;
		}
		case LogMessage::lnf: {
			if (Log::LastMessage != lm::end) {
				if (Log::LastMessage == lm::lnf) {
					Log::LastMessage = lm::lnf;
					this->Tout << _T('\n');
				}
				else {
					Log::LastMessage = lm::lnf;
					this->Tout << _T('\n') << ANSIESC_RESET; //还原颜色
				}
			}

			break;
		}
		}

		return _Tostream;
	}

	template<class Temp = bool>
	void Log_README()
	{
		lgcr <<
			_T("#define _ANSIESC_CONSOLE_CHAR_COLOR: 控制台转义字符修改字符颜色\n\
\n\
		#ifndef _DEBUG: _WINDOWS/_CONSOLE\n\
		lgr("文件打开失败!", Terr); \n\
		_DEBUG: _WINDOWS/_CONSOLE\n\
		lg("关闭窗口", tip); \n\
\n\
		#ifndef _DEBUG: _CONSOLE\n\
		lgcr(\"Code: 404\", Terr); \n\
		lgcr << tip << \"关闭文件!\'n\'\" << end; \n\
		//_DEBUG: _CONSOLE\n\
		lgc(\"Log.h\", lm::txt); \n\
		lgc << war << \"没有对应文件!\"\n")
			<< end;
	}
}


#endif
