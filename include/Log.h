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

#if 0
#ifdef _WINDOWS

#define PATH_BACKSLASH "\\"
#else

#define PATH_BACKSLASH "/"
#endif

#endif
#define PATH_BACKSLASH "/"

#define Log_nl "temp"
#define Log_ts "[INFO]"
#define Log_wr "[WARNING]"
#define Log_er "[ERROR]"

#define ANSI_RESET "\033[0m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_RED "\033[31m"

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
		 

#ifdef _WINDOWS
		static HWND hConsole;
#endif

		static std::ofstream LogFileStream_Out;
		static bool LogFileWrite;
		static bool LogAllOutput;

#ifdef _Thread
		static std::mutex mutex_LogFileStream_Out; //保护 LogFileStream_Out/LogFileWrite_Queue
		static std::queue<Ustr> LogFileWrite_Queue; //日志文件写入队列
		static std::thread LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
		static std::atomic<bool> LogFileWriteThreadStop;
		static std::condition_variable cv_LogFileQueue;
#endif

	public:
		Log(bool cmd, bool release)
			: CMD(cmd), Release(release), ShowTime(true), ShowLog(true)
		{}

		~Log();

	public:
		/*
		* 控制台初始化状态: false(需要初始化), true(跳过初始化)
		*/
		template<class Temp = bool>
		void Init()
		{
			try {
				//控制台初始化
				if (!FirstInit)
				{
					// 设置全局区域为 "zh_CN.UTF-8"
					std::locale::global(std::locale("zh_CN.UTF-8"));

					// 使用区域设置进行格式化
					Ucout.imbue(std::locale()); // 使用全局区域设置
					Ucerr.imbue(std::locale());

					//Ucout << "Log 区域设置: zh_CN.UTF-8 完成!\n";

#ifdef _WINDOWS

				//Windows 控制台编码修改: UTF-8
					SetConsoleOutputCP(CP_UTF8);
					SetConsoleCP(CP_UTF8);

					//分离控制台
					/*if (FreeConsole() == 0)
					{
						MessageBox(0, "log: 分离控制台失败!", Log_er, MB_ICONSTOP);
						MessageBox(0, "错误代码: " + Uto_string(GetLastError()).c_str(), Log_er, MB_ICONSTOP);
					}*/

					//分配控制台: 当不是控制台程序时
#ifndef _CONSOLE
					if (AllocConsole() == 0)
					{
						MessageBox(0, "log: 分配控制台失败!", Log_er, MB_ICONSTOP);
						MessageBox(0, ((Ustr)"错误代码: " + Uto_string(GetLastError())).c_str(), Log_er, MB_ICONSTOP);
					}
#endif

					// 获取控制台窗口的句柄
					hConsole = GetConsoleWindow();
#endif
					if (LogFileWrite) {
						//Ucerr << "Log 日志文件写入: 开始\n";

						//获取 当前路径/Log/Log文件名.txt 
						//创建文件夹 ./Log  .
						Ustr Log_FolderName = (Ustr)"." + PATH_BACKSLASH + "Log";
						if (CreateFolder(Log_FolderName)) {
						}
						else {
							Ucerr << ANSI_YELLOW
								<< "Log: 文件夹 " + Log_FolderName + "创建失败!\n"
								<< ANSI_RESET;

							Log_FolderName = (Ustr)".";
						}

						//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
						Ustr Log_FileName = GetFormattingTime("%Y-%m-%d_%H-%M-%S", "", "") + ".txt";
						// ./Log/时间.txt  ||  ./时间.txt
						Ustr Log_FilePath = Log_FolderName + PATH_BACKSLASH + Log_FileName; //总是添加 /
																					
						try {
							//打开文件
							Ustr _文件编码 = "UTF-8BOM";
							//Ucerr << "Log: 日志输出文件名 " << Log_FilePath << "\n" << std::endl;
							LogFileStream_Out = std::move(std::ofstream(Log_FilePath, std::ios::out));
							if (!LogFileStream_Out) {
								Ucerr << ANSI_RED
									<< (Ustr)"Log: " + Log_FilePath + " 打开文件失败!\n"
									<< ANSI_RESET;
								return;
							}
							Ucerr << ANSI_GREEN
								<< (Ustr)"Log: " + Log_FilePath + " 打开文件成功!\n"
								<< ANSI_RESET;
							//Ucerr << "Log: 文件 开始写入!\n";

							if (_文件编码 == "UTF-8BOM") {
								// 写入 UTF-8 BOM (EF BB BF)
								Ucerr << "Log: 写入 UTF-8BOM 编码(EF BB BF)\n";

								LogFileStream_Out.put(0xEF);
								LogFileStream_Out.put(0xBB);
								LogFileStream_Out.put(0xBF);
							}
						}
						catch (...) {
							Ucerr << "Typical_Tool::Log::Init()::LogFileStream_Out\n";
						}

#ifdef _Thread
						//初始化: 日志文件写入线程
						try {
							LogFileProcessing = std::thread(&Log::LogWirteToFile,
								this);
							LogFileProcessing.detach(); //分离线程
							Ucout << "Log: 日志写入线程 分离到后台\n";
						}
						catch (...) {
							Ucerr << "Typical_Tool::Log::Init()::LogFileProcessing\n";
						}
#endif
					}
					Ucout << std::endl;

					//完成初始化
					FirstInit = true;
				}
			}
			catch (...) {
				Ucerr << "void Typical_Tool::Log::Init()\n";
			}
		}

	private:
		void LogWirteToFile()
		{

#ifdef _Thread
			try {
				//不是停止线程时
				while (!LogFileWriteThreadStop.load()) {
					//锁 日志文件流
					std::unique_lock<std::mutex> queueLock(mutex_LogFileStream_Out); // 上锁
					cv_LogFileQueue.wait(queueLock, [this] {
						return !LogFileWrite_Queue.empty() || LogFileWriteThreadStop.load();
					});

				if (LogFileWriteThreadStop.load() && LogFileWrite_Queue.empty()) {
					break; // 退出循环
				}
					
					//日志队列非空
					if (!LogFileWrite_Queue.empty()) {
						LogFileStream_Out << LogFileWrite_Queue.front();
						LogFileWrite_Queue.pop();
					}
					else { //日志为空: 等待 200ms
						queueLock.unlock();
						std::this_thread::sleep_for(std::chrono::milliseconds(200));
					}

				}
			}
			catch (const std::exception& e) {
				Ucerr << "Typical_Tool::Log::LogWriteToFile() - " << e.what() << "\n";
			}
			catch (...) {
				Ucerr << "Typical_Tool::Log::LogWirteToFile()\n";
			}
#endif
		}

		template<class T = bool>
		void StopLogWrite()
		{
#ifdef _Thread
			LogFileWriteThreadStop.store(true);
			cv_LogFileQueue.notify_one(); // 通知线程退出
			Ucout << "Log: 日志写入线程 退出\n" << std::endl;
#endif
		}

	private:

		template<class T = bool>
		void SetConsoleTextColor(const Ustr& _Color)
		{
			Ucout << _Color; //修改颜色
		}
		template<class T = bool>
		void SetConsoleTextColor_Error(const Ustr& _Color)
		{
			Ucerr << _Color; //修改颜色
		}
		template<class T = bool>
		void ReSetConsoleTextColor()
		{
			Ucout << ANSI_RESET; //还原颜色
		}
		template<class T = bool>
		void ReSetConsoleTextColor_Error()
		{
			Ucerr << ANSI_RESET; //还原颜色
		}

		template<class T = bool>
		void ConsoleOutput(const Ustr& _Test)
		{
			Ucout << _Test;
		}
		template<class T = bool>
		void ConsoleOutput_Error(const Ustr& _Test)
		{
			Ucerr << _Test;
		}


		// _WINDOWS || _CONSOLE
		template<class T = bool>
		void Logs_ustr_ustr(const Ustr& text, const Ustr& title)
		{
			if (CMD)
			{
				Ustr temp;
				/*if (ShowTime) {
					Ucout << GetFormattingTime();
				}*/
				temp = title + ": " + text;
				ConsoleOutput(temp);

				//文件写入 log日志
				if (LogFileWrite) {
#ifdef _Thread
					{
						//锁 日志文件写入队列
						std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

						if (LogAllOutput) {
							LogFileWrite_Queue.push(temp);
						}
					}
					cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
					if (ShowTime) {
						LogFileStream_Out << GetFormattingTime();
					}
					LogFileStream_Out << temp;
				}

				return;
			}
#ifdef _WINDOWS
			MessageBox(NULL, text.c_str(), title.c_str(), MB_OK);
#endif
		}
	
		template<class T = bool>
		void Logs_ustr(const Ustr& text, const LogMessage& lgm)
		{
			switch (lgm)
			{
			case LogMessage::ts:
			{
				if (CMD)
				{
					Ustr temp;
					SetConsoleTextColor(ANSI_GREEN); //在时间输出之前
					/*if (ShowTime) {
						Ucout << GetFormattingTime();
					}*/
					temp = (Ustr)Log_ts + " " + text;
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//文件写入 log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志文件写入队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (ShowTime) {
							LogFileStream_Out << GetFormattingTime();
						}
						LogFileStream_Out << temp;
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
					/*if (ShowTime) {
						Ucout << GetFormattingTime();
					}*/
					temp = (Ustr)Log_wr + " " + text;
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//文件写入 log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志文件写入队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (ShowTime) {
							LogFileStream_Out << GetFormattingTime();
						}
						LogFileStream_Out << temp;
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
					/*if (ShowTime) {
						Ucout << GetFormattingTime();
					}*/
					temp = (Ustr)Log_er + " " + text;
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//文件写入 log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志文件写入队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁
							LogFileWrite_Queue.push(temp);
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (ShowTime) {
							LogFileStream_Out << GetFormattingTime();
						}
						LogFileStream_Out << temp;
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
					/*if (ShowTime) {
						Ucout << GetFormattingTime();
					}*/
					temp = text;
					ConsoleOutput(temp);

					//文件写入 log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志文件写入队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (ShowTime) {
							LogFileStream_Out << GetFormattingTime();
						}
						LogFileStream_Out << temp;
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
		void Logs_lgm(const LogMessage& lgm)
		{
			if (lgm == LogMessage::nl)
			{
				if (CMD)
				{
					Ustr temp = "\n";
					ConsoleOutput(temp);

					//文件写入 log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志文件写入队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (ShowTime) {
							LogFileStream_Out << GetFormattingTime();
						}
						LogFileStream_Out << temp;
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
		void operator()(const Ustr& text, const LogMessage& lgm = lgm::nl)
		{
			Init();
			if (ShowLog)
			{
				Logs(text, lgm);
			}
		}
		template<class T = bool>
		void operator()(Ustr&& text, const LogMessage& lgm = lgm::nl)
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

		// _WINDOWS: Win32 API
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
					Ucout << "Log: 文件夹 " + folderPath + " 创建成功!" + "\n";
					// 创建成功
					return true;
				}
				Ucerr << "Log: 文件夹 " + folderPath + " 创建失败!" + "\n";
				// 创建失败且不是因为路径已存在  
				return false;
			}
			else if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				Ucout << "Log: 文件夹" + folderPath + " 已存在" + "\n";
				// 路径已经是一个目录  
				return true;
			}
			// 路径存在但不是目录（可能是一个文件）  
			Ucout << "Log: 文件夹 " + folderPath + " 创建失败(路径存在, 但不是目录)!" + "\n";
#else
			Ucout << "Log: 文件夹 " + folderPath + " 创建失败(#ifndef _WINDOWS)!" + "\n";
#endif
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
		Ustr GetFormattingTime(const Ustr& timeFormat = "%Y-%m-%d %H:%M:%S",
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

	public:

		/*
		* level:
		* er: Error log level output
		* !=3: All(lgm: ts/wr/nl) log level output
		*/
		template<class T = bool>
		static void SetAllLogFileWrite(bool logFileWrite, int logLevel = lgm::er)
		{
			LogFileWrite = logFileWrite;

			//输出所有级别
			if (logLevel != lgm::er) {
				LogAllOutput = true;

				Ucout << ANSI_YELLOW 
					<< "Log: File Output level >> All <<"
					<< ANSI_RESET << std::endl;
			}
			else {
				LogAllOutput = false;

				Ucout << ANSI_YELLOW
					<< "Log: File Output level >> Error <<"
					<< ANSI_RESET << std::endl;
			}

			Ucout << std::endl;
		}
#define 设置所有日志写入 SetAllLogFileWrite

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
		lgcr("\n_WINDOWS", lgm::nl);
		lgcr(lgm::nl);
		lgcr("\nTypical_Tool::README()", lgm::nl);
		lgcr("\n 支持 Unicode: #define UNICODE", lgm::nl);
		lgcr("\n 使用方法-函数对象, 代码如下:", lgm::nl);
		lgcr("\n 输出[_CONSOLE | _WINDOWS](_DEBUG)", "lg()");
		lgcr("\n 输出[_CONSOLE | _WINDOWS](_DEBUG | Release)", "lgr()");
		lgcr("\n CMD输出[_WINDOWS](_DEBUG)", "lgc()");
		lgcr("\n CMD输出[_WINDOWS](_DEBUG | Release)", "lgcr()");
		lgcr(lgm::nl);
		lgcr("lgm", "enum");
		lgcr("\t#ifndef _English", lgm::nl);
		lgcr("\tlgm::nl,  //空标题(不带标题): 只输出内容", lgm::nl);
		lgcr("\tlgm::ts,  //提示", lgm::nl);
		lgcr("\tlgm::wr,  //警告", lgm::nl);
		lgcr("\tlgm::er   //错误", lgm::nl);
		lgcr(lgm::nl);
		lgcr("\n日志显示开关", lgm::nl);
		lgcr("\tTypical_Tool::设置日志显示(bool)", lgm::nl);
		lgcr("\tTypical_Tool::设置时间显示(bool)", lgm::nl);
		lgcr("\tTypical_Tool::Log::设置日志显示(bool)", lgm::nl);
		lgcr("\tTypical_Tool::Log::设置时间显示(bool)", lgm::nl);
		lgcr(lgm::nl);
		lgcr(lgm::nl);
		lgcr(lgm::nl);
		lgcr(lgm::nl);
		lgcr("\nTypical_Tool::README()", lgm::nl);
		lgcr("\n Unicode support: #define UNICODE", lgm::nl);
		lgcr("\n Using method - function object, the code is as follows:", lgm::nl);
		lgcr("\n Print [_CONSOLE | _WINDOWS](_DEBUG)", "lg()");
		lgcr("\n Print [_CONSOLE | _WINDOWS](_DEBUG | Release)", "lgr()");
		lgcr("\n Command Print [_WINDOWS](_DEBUG)", "lgc()");
		lgcr("\n Command Print [_WINDOWS](_DEBUG | Release)", "lgcr()");
		lgcr(lgm::nl);
		lgcr(lgm::nl);
		lgcr("lgm", "enum");
		lgcr("\t#ifdef _English", lgm::nl);
		lgcr("\tlgm::nl,  //Empty title (without title): Only the content is output", lgm::nl);
		lgcr("\tlgm::ts,  //Tips", lgm::nl);
		lgcr("\tlgm::wr,  //Warning", lgm::nl);
		lgcr("\tlgm::er   //Error", lgm::nl);
		lgcr(lgm::nl);
		lgcr("\nLog display switch:", lgm::nl);
		lgcr("\tTypical_Tool::SetShowLog(bool)", lgm::nl);
		lgcr("\tTypical_Tool::SetConsoleTimeShow(bool)", lgm::nl);
		lgcr("\tTypical_Tool::Log::SetShowLog(bool)", lgm::nl);
		lgcr("\tTypical_Tool::Log::SetConsoleTimeShow(bool)", lgm::nl);
	}
}


#endif