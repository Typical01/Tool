#pragma once

#include "pch.h"

#ifndef _LOG_H
#define _LOG_H

#define _CRT_SECURE_NO_WARNINGS

//#define _CONSOLE
//#undef _WINDOWS

//#include "Typical_Tool.h"

#ifndef _WINDOWS
	// 定义虚拟类型
	using DWORD = unsigned long;
	using WORD = unsigned short;
	using HANDLE = void*;
#endif

namespace Typical_Tool
{
	enum  LogMessage
	{
		nl, //空行: NULL String
		ts, //提示: Tips
		wr, //警告: Warning
		er  //错误: Error
	};
	typedef LogMessage lgm;

#ifndef UNICODE
	#define Uchar char
	#define Ustr std::string
	#define Ucout std::cout
	#define Ucerr std::cerr
	#define Uto_string std::to_string
	#define Ustrlen std::strlen
	#define Uostringstream std::ostringstream
	#define Ufopen_s fopen_s
	#define Ufputs fputs
	#define Ufgets fgets

#else

	#define Uchar wchar_t
	#define Ustr std::wstring
	#define Ucout std::wcout
	#define Ucerr std::wcerr
	#define Uto_string std::to_wstring
	#define Ustrlen std::wcslen
	#define Uostringstream std::wostringstream
	#define Ufopen_s _wfopen_s
	#define Ufputs fputws
	#define Ufgets fgetws

#endif


#ifndef UNICODE
	// ""
	#define _T(x) x
#else
	// L""
	#define _T(x) L ## x
#endif

#ifdef _WINDOWS

#define PATH_BACKSLASH _T("\\")
#else

#define PATH_BACKSLASH _T("/")
#endif

#define Log_nl _T("temp")
#define Log_ts _T("[INFO]")
#define Log_wr _T("[WARNING]")
#define Log_er _T("[ERROR]")

#define ANSI_RESET _T("\033[0m")
#define ANSI_GREEN _T("\033[32m")
#define ANSI_YELLOW _T("\033[33m")
#define ANSI_RED _T("\033[31m")


#ifdef _WINDOWS
	 extern HWND hConsole;
#endif

	 extern std::vector<class Log> Log_All;

	 extern std::mutex mutex_LogFileStream; //保护 LogFileStream/LogFileWrite_Queue
	 extern FILE* LogFileStream;
	 extern bool LogFileWrite;
	 extern bool LogAllOutput;
	 
	 extern std::queue<Ustr> LogFileWrite_Queue; //日志文件写入队列
	 extern std::thread LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
	 
	 extern bool LogFileWriteThreadStop;

	/// <summary>
	/// 非线程安全
	/// </summary>
	class  Log {
	private:
		static bool FirstInit;

		bool CMD;
		bool Release;

		bool ShowTime;
		bool ShowLog;
		 
	public:
		Log(bool cmd, bool release)
			: CMD(cmd), Release(release), ShowTime(true), ShowLog(true) {	}

		~Log();

	public:
		/*
		* 控制台初始化状态: false(需要初始化), true(跳过初始化)
		*/
		template<class Temp = bool>
		void Init()
		{
			//控制台初始化
			if (!FirstInit)
			{
				Ucout.imbue(std::locale(""));

#ifndef _CONSOLE
#ifdef _WINDOWS
				////分离控制台
				//if (FreeConsole() == 0)
				//{
				//	MessageBox(0, _T("log: 分离控制台失败!"), Log_er, MB_ICONSTOP);
				//	MessageBox(0, _T("错误代码: ") + Uto_string(GetLastError()).c_str(), Log_er, MB_ICONSTOP);
				//}

				//分配控制台
				if (AllocConsole() == 0)
				{
					MessageBox(0, _T("log: 分配控制台失败!"), Log_er, MB_ICONSTOP);
					MessageBox(0, ((Ustr)_T("错误代码: ") + Uto_string(GetLastError())).c_str(), Log_er, MB_ICONSTOP);
				}

				// 获取控制台窗口的句柄
				hConsole = GetConsoleWindow();
#endif
#endif
				if (LogFileWrite) {
#ifdef _WINDOWS
					//获取 当前路径/Log/Log文件名.txt 
					//创建文件夹 Log
					Ustr Log_FolderName = (Ustr)_T(".") + PATH_BACKSLASH + _T("Log");
					if (CreateFolder(Log_FolderName)) {
						Ucout << _T("Log 文件夹: 创建成功!\n\n");
					}
					else {
						Ucout << _T("Log 文件夹: 创建失败!\n\n");
						Log_FolderName = (Ustr)_T(".") + PATH_BACKSLASH + _T("");
					}

					//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + _程序名.txt
					//需要 多线程支持
					Ustr Log_FileName = GetFormattingTime(_T("%Y-%m-%d_%H-%M-%S")) + _T(".txt");

					Ustr Log_FilePath = Log_FolderName + PATH_BACKSLASH + Log_FileName;
					//打开文件
					errno_t err = Ufopen_s(&LogFileStream, Log_FilePath.c_str(), _T("a+, ccs=UTF-8")); //追加模式打开文件
					if (err != 0)
					{
						Ucout << (Ustr)_T("文件: ") + Log_FilePath + _T(" 打开错误!") << std::endl;
						if (LogFileStream == nullptr) {
							Ucout << (Ustr)_T("LogFileStream errno_t: ") + Uto_string(err) << std::endl;
						}
						//return false; //没有文件, 则不进行读取和解析
					}
					Ucout << (Ustr)_T("文件: ") + Log_FilePath + _T(" 打开成功") << std::endl;
#endif
				}

				//初始化: 日志文件写入线程
				LogFileProcessing = std::thread(&Log::LogWirteToFile, this, std::ref(LogFileWrite_Queue), LogFileStream, std::ref(LogFileWriteThreadStop));
				LogFileProcessing.detach(); //分离线程

				//完成初始化
				FirstInit = true; 
			}
			else { //跳过初始化
				return;
			}
		}

	private:
		void LogWirteToFile(std::queue<Ustr>& _LogFileWrite_Queue, FILE* _LogFileStream, bool& _LogFileWriteThreadStop)
		{
			try {
				//不是停止线程时
				while (!_LogFileWriteThreadStop) {
					//锁 日志文件流
					std::lock_guard<std::mutex> lock(mutex_LogFileStream); // 上锁
					
					//日志队列非空
					if (!_LogFileWrite_Queue.empty()) {
						Ufputs(_LogFileWrite_Queue.front().c_str(), _LogFileStream);
						_LogFileWrite_Queue.pop();
					}
					else { //日志为空: 等待 20ms
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
					}

				}
			}
			catch (...) {
				Ucerr << _T("Error: Typical_Tool::Log::LogWirteToFile()");
			}
		}

	private:

		template<class T = bool>
		void SetConsoleTextColor(const Ustr& _Color)
		{
			Ucout << _Color << _T("_"); //修改颜色
		}
		template<class T = bool>
		void SetConsoleTextColor_Error(const Ustr& _Color)
		{
			Ucerr << _Color << _T("_"); //修改颜色
		}
		template<class T = bool>
		void ReSetConsoleTextColor()
		{
			Ucout << _T("_") << ANSI_RESET << _T("\n"); //还原颜色
		}
		template<class T = bool>
		void ReSetConsoleTextColor_Error()
		{
			Ucerr << _T("_") << ANSI_RESET << _T("\n"); //还原颜色
		}

		template<class T = bool>
		void ConsoleOutput(const Ustr& _Test)
		{
			Ucout << _Test << _T("\n");
		}
		template<class T = bool>
		void ConsoleOutput_Error(const Ustr& _Test)
		{
			Ucerr << _Test << _T("\n");
		}


		// _WINDOWS || _CONSOLE
		template<class T = bool>
		void Logs_ustr_ustr(const Ustr& text, const Ustr& title)
		{
			if (CMD)
			{
				Ustr temp;
				if (ShowTime) {
					Ucout << GetFormattingTime();
				}
				temp = title + _T(": ") + text;
				ConsoleOutput(temp);

				//文件写入 log日志
				if (LogFileWrite) {
					//锁 日志文件写入队列
					std::lock_guard<std::mutex> lock(mutex_LogFileStream); // 上锁

					if (LogAllOutput) {
						LogFileWrite_Queue.push(temp);
					}
				}

				return;
			}
#ifdef _WINDOWS
			MessageBox(NULL, text.c_str(), title.c_str(), MB_OK);
#endif
		}
	
		template<class T = bool>
		void Logs_ustr(const Ustr& text, LogMessage lgm)
		{
			switch (lgm)
			{
			case LogMessage::ts:
			{
				if (CMD)
				{
					Ustr temp;
					SetConsoleTextColor(ANSI_GREEN); //在时间输出之前
					if (ShowTime) {
						Ucout << GetFormattingTime();
					}
					temp = (Ustr)Log_ts + _T(": ") + text;
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//文件写入 log日志
					if (LogFileWrite) {
						//锁 日志文件写入队列
						std::lock_guard<std::mutex> lock(mutex_LogFileStream); // 上锁

						if (LogAllOutput) {
							LogFileWrite_Queue.push(temp);
						}
					}

					break;
				}
#ifdef _WINDOWS
				MessageBox(NULL, text.c_str(), Log_ts, MB_OK);
#endif

				break;
			}
			case LogMessage::wr:
			{
				if (CMD)
				{
					Ustr temp;
					SetConsoleTextColor(ANSI_YELLOW); //在时间输出之前
					if (ShowTime) {
						Ucout << GetFormattingTime();
					}
					temp = (Ustr)Log_wr + _T(": ") + text;
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//文件写入 log日志
					if (LogFileWrite) {
						//锁 日志文件写入队列
						std::lock_guard<std::mutex> lock(mutex_LogFileStream); // 上锁

						if (LogAllOutput) {
							LogFileWrite_Queue.push(temp);
						}
					}

					break;
				}
#ifdef _WINDOWS
				MessageBox(NULL, text.c_str(), Log_wr, MB_ICONWARNING);
#endif

				break;
			}
			case LogMessage::er:
			{
				if (CMD)
				{
					Ustr temp;
					SetConsoleTextColor(ANSI_RED); //在时间输出之前
					if (ShowTime) {
						Ucout << GetFormattingTime();
					}
					temp = (Ustr)Log_er + _T(": ") + text;
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//文件写入 log日志
					if (LogFileWrite) {
						//锁 日志文件写入队列
						std::lock_guard<std::mutex> lock(mutex_LogFileStream); // 上锁

						LogFileWrite_Queue.push(temp);
					}

					break;
				}
#ifdef _WINDOWS
				MessageBox(NULL, text.c_str(), Log_er, MB_ICONSTOP);
#endif

				break;
			}
			case LogMessage::nl:
			{
				if (CMD)
				{
					Ustr temp;
					if (ShowTime) {
						Ucout << GetFormattingTime();
					}
					temp = text;
					ConsoleOutput(temp);

					//文件写入 log日志
					if (LogFileWrite) {
						//锁 日志文件写入队列
						std::lock_guard<std::mutex> lock(mutex_LogFileStream); // 上锁

						if (LogAllOutput) {
							LogFileWrite_Queue.push(temp);
						}
					}

					break;
				}
#ifdef _WINDOWS
				MessageBox(NULL, text.c_str(), Log_nl, MB_OK);
#endif
				
				break;
			}
			}
		}
		template<class T = bool>
		void Logs_lgm(LogMessage lgm)
		{
			if (lgm == LogMessage::nl)
			{
				if (CMD)
				{
					Ustr temp = _T("\n");
					ConsoleOutput(temp);

					//文件写入 log日志
					if (LogFileWrite) {
						//锁 日志文件写入队列
						std::lock_guard<std::mutex> lock(mutex_LogFileStream); // 上锁

						if (LogAllOutput) {
							LogFileWrite_Queue.push(temp);
						}
					}

					return;
				}
			}
		}


		// _DEBUG || NDEBUG
		template<class T = bool>
		void Logs(const Ustr& text, const Ustr& title)
		{
#ifdef _DEBUG
			Logs_ustr_ustr(text, title);
			return;
#endif
			if (Release)
			{
				Logs_ustr_ustr(text, title);
			}
		}
		template<class T = bool>
		void Logs(const Ustr& text, const LogMessage& lgm)
		{
#ifdef _DEBUG
			Logs_ustr(text, lgm);
			return;
#endif
			if (Release)
			{
				Logs_ustr(text, lgm);
			}
		}
		template<class T = bool>
		void Logs(const LogMessage& lgm)
		{
#ifdef _DEBUG
			Logs_lgm(lgm);
			return;
#endif
			if (Release)
			{
				Logs_lgm(lgm);
			}
		}

	public:

		//显示/隐藏 Log消息
		template<class T = bool>
		void operator()(const Ustr& text, const Ustr& title)
		{
			Init();
			if (ShowLog)
			{
				Logs(text, title);
			}
		}
		template<class T = bool>
		void operator()(Ustr&& text, Ustr&& title)
		{
			Init();
			if (ShowLog)
			{
				Logs(text, title);
			}
		}
		template<class T = bool>
		void operator()(const Ustr& text, Ustr&& title)
		{
			Init();
			if (ShowLog)
			{
				Logs(text, title);
			}
		}
		template<class T = bool>
		void operator()(Ustr&& text, const Ustr& title)
		{
			Init();
			if (ShowLog)
			{
				Logs(text, title);
			}
		}
		template<class T = bool>
		void operator()(const Ustr& text, const LogMessage& lgm = lgm::nl)
		{
			Init();
			if (ShowLog)
			{
				Logs(text, lgm);
			}
		}
		template<class T = bool>
		void operator()(Ustr&& text, LogMessage lgm = lgm::nl)
		{
			Init();
			if (ShowLog)
			{
				Logs(text, lgm);
			}
		}
		template<class T = bool>
		void operator()(const LogMessage& lgm = lgm::nl)
		{
			Init();
			if (ShowLog)
			{
				Logs(lgm);
			}
		}


	private:

		// 创建文件夹: Win32 API
		template<class T = bool>
		bool CreateFolder(const Ustr& folderPath)
		{
#ifdef _WINDOWS
			DWORD attributes = GetFileAttributes(folderPath.c_str());

			// 检查路径是否存在且不是目录  
			if (attributes == INVALID_FILE_ATTRIBUTES)
			{
				// 路径不存在或出错，尝试创建目录  
				if (CreateDirectory(folderPath.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
				{
					//lgc(_T("文件夹") + folderPath + _T(("创建成功(或已存在)!")));
					// 创建成功或路径已存在  
					return true;
				}
				//lgc(_T("文件夹") + folderPath + _T("创建失败!"));
				// 创建失败且不是因为路径已存在  
				return false;
			}
			else if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//lgc(_T("文件夹") + folderPath + _T("已存在"));
				// 路径已经是一个目录  
				return true;
			}
#endif
			//lgc(_T("文件夹") + folderPath + _T(("创建失败(路径存在, 但不是目录)!")));
			// 路径存在但不是目录（可能是一个文件）  
			return false;
		}


	public:

		void SetShowLog(bool showLog);
#define 设置日志显示 SetShowLog
		void SetShowTime(bool showTime);
#define 设置时间显示 SetShowTime

		// _WINDOWS: Win32 API
		void SetShowConsole(bool showConsole, bool isTips = false);
#define 设置控制台显示 SetShowConsole


	private:
		template<class Temp = bool>
		Ustr GetFormattingTime(const Ustr& timeFormat = _T("%Y-%m-%d %H:%M:%S"),
			const Ustr& textLeftFormat = _T("["), const Ustr& textRigthFormat = _T("]"))
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
			if (textLeftFormat == _T("") && textRigthFormat == _T("")) {
				return oss.str();
			}
			return textLeftFormat + oss.str() + textRigthFormat;
		}

	public:

		template<class T = bool>
		static void SetAllShowLog(bool showLog, std::vector<Log>& _Log_All = Log_All)
		{
			if (showLog) {
				Ucout << _T("日志-输出显示: 显示");
			}
			else {
				Ucout << _T("日志-输出显示: 隐藏");
			}

			for (auto log : _Log_All) {
				log.SetShowLog(showLog);
			}
		}
#define 设置日志显示_全部 SetAllShowLog
		template<class T = bool>
		static void SetAllShowTime(bool showTime, std::vector<Log>& _Log_All = Log_All)
		{
			if (showTime) {
				Ucout << _T("日志-时间显示: 显示");
			}
			else {
				Ucout << _T("日志-时间显示: 隐藏");
			}

			for (auto log : _Log_All) {
				log.SetShowTime(showTime);
			}
		}
#define 设置时间显示_全部 SetAllShowTime

		// _WINDOWS: Win32 API
		template<class T = bool>
		static void SetAllShowConsole(bool showConsole, bool isTips = false, std::vector<Log>& _Log_All = Log_All)
		{
#ifdef _WINDOWS
			if (showConsole) {
				if (isTips) {
					MessageBox(0, _T("Windows: Show CMD"), Log_wr, MB_ICONSTOP);
				}
				else {
					MessageBox(0, _T("Windows: Not Show CMD"), Log_wr, MB_ICONSTOP);
				}
			}

			for (auto log : _Log_All) {
				log.SetShowConsole(showConsole, isTips);
			}
#endif
		}
#define 设置控制台显示_全部 SetAllShowConsole

		/*
		* level:
		* er: Error log level output
		* !=3: All log level output
		*/
		template<class T = bool>
		static void SetLogFileWrite(bool logFileWrite, int logLevel = lgm::er)
		{
			LogFileWrite = logFileWrite;

			//输出所有级别
			if (logLevel != lgm::er) {
				LogAllOutput = true;

				SetConsoleTextColor(ANSI_YELLOW);
				ConsoleOutput(_T("[日志-文件]: 输出级别 >> 所有 <<"));
				ReSetConsoleTextColor();
			}
			else {
				LogAllOutput = false;

				SetConsoleTextColor(ANSI_YELLOW);
				ConsoleOutput_Error(_T("[日志-文件]: 输出级别 >> 错误 <<"));
				ReSetConsoleTextColor();
			}
		}
#define 设置日志写入 SetLogFileWrite

	};

	//模式 mode: (_CONSOLE | _WINDOWS) && #ifdef _DEBUG
	static Log lg(false, false);
	//模式 mode: (_CONSOLE) && #ifdef _DEBUG
	static Log lgc(true, false);
	//模式 mode: (_CONSOLE | _WINDOWS) && #ifndef _DEBUG
	static Log lgr(false, true);
	//模式 mode: (_CONSOLE) && #ifndef _DEBUG
	static Log lgcr(true, true);

	template<class Temp = bool>
	void Log_README()
	{
		lgcr(_T("_WINDOWS"), lgm::nl);
		lgcr(lgm::nl);
		lgcr(_T("Typical_Tool::README()"), lgm::nl);
		lgcr(_T(" 支持 Unicode: #define UNICODE"), lgm::nl);
		lgcr(_T(" 使用方法-函数对象, 代码如下:"), lgm::nl);
		lgcr(_T(" 输出[_CONSOLE | _WINDOWS](_DEBUG)"), _T("lg()"));
		lgcr(_T(" 输出[_CONSOLE | _WINDOWS](_DEBUG | Release)"), _T("lgr()"));
		lgcr(_T(" CMD输出[_WINDOWS](_DEBUG)"), _T("lgc()"));
		lgcr(_T(" CMD输出[_WINDOWS](_DEBUG | Release)"), _T("lgcr()"));
		lgcr(lgm::nl);
		lgcr(_T("lgm"), _T("enum"));
		lgcr(_T("\t#ifndef _English"), lgm::nl);
		lgcr(_T("\tlgm::nl,  //空标题(不带标题): 只输出内容"), lgm::nl);
		lgcr(_T("\tlgm::ts,  //提示"), lgm::nl);
		lgcr(_T("\tlgm::wr,  //警告"), lgm::nl);
		lgcr(_T("\tlgm::er   //错误"), lgm::nl);
		lgcr(lgm::nl);
		lgcr(_T("日志显示开关"), lgm::nl);
		lgcr(_T("\tTypical_Tool::设置日志显示(bool)"), lgm::nl);
		lgcr(_T("\tTypical_Tool::设置时间显示(bool)"), lgm::nl);
		lgcr(_T("\tTypical_Tool::Log::设置日志显示(bool)"), lgm::nl);
		lgcr(_T("\tTypical_Tool::Log::设置时间显示(bool)"), lgm::nl);
		lgcr(lgm::nl);
		lgcr(lgm::nl);
		lgcr(lgm::nl);
		lgcr(lgm::nl);
		lgcr(_T("Typical_Tool::README()"), lgm::nl);
		lgcr(_T(" Unicode support: #define UNICODE"), lgm::nl);
		lgcr(_T(" Using method - function object, the code is as follows:"), lgm::nl);
		lgcr(_T(" Print [_CONSOLE | _WINDOWS](_DEBUG)"), _T("lg()"));
		lgcr(_T(" Print [_CONSOLE | _WINDOWS](_DEBUG | Release)"), _T("lgr()"));
		lgcr(_T(" Command Print [_WINDOWS](_DEBUG)"), _T("lgc()"));
		lgcr(_T(" Command Print [_WINDOWS](_DEBUG | Release)"), _T("lgcr()"));
		lgcr(lgm::nl);
		lgcr(lgm::nl);
		lgcr(_T("lgm"), _T("enum"));
		lgcr(_T("\t#ifdef _English"), lgm::nl);
		lgcr(_T("\tlgm::nl,  //Empty title (without title): Only the content is output"), lgm::nl);
		lgcr(_T("\tlgm::ts,  //Tips"), lgm::nl);
		lgcr(_T("\tlgm::wr,  //Warning"), lgm::nl);
		lgcr(_T("\tlgm::er   //Error"), lgm::nl);
		lgcr(lgm::nl);
		lgcr(_T("Log display switch:"), lgm::nl);
		lgcr(_T("\tTypical_Tool::SetShowLog(bool)"), lgm::nl);
		lgcr(_T("\tTypical_Tool::SetConsoleTimeShow(bool)"), lgm::nl);
		lgcr(_T("\tTypical_Tool::Log::SetShowLog(bool)"), lgm::nl);
		lgcr(_T("\tTypical_Tool::Log::SetConsoleTimeShow(bool)"), lgm::nl);
	}
}


#endif