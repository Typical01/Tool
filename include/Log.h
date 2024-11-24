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
		tx, //文本: Text
		lf, //换行: Line Feed
		ts, //提示: Tips [INFO]
		wr, //警告: Warning [WARRING]
		er  //错误: Error [ERROR]
	};
	typedef LogMessage lm;

#ifndef UNICODE
	#define Tchar char
	#define Tstr std::string
	#define Tcout std::cout
	#define Tcerr std::cerr
	#define Tto_string std::to_string
	#define Tstrlen std::strlen
	#define Tostringstream std::ostringstream
	#define Tfopen_s fopen_s
	#define Tfputs fputs
	#define Tfgets fgets

#else
	#define Tchar wchar_t
	#define Tstr std::wstring
	#define Tcout std::wcout
	#define Tcerr std::wcerr
	#define Tto_string std::to_wstring
	#define Tstrlen std::wcslen
	#define Tostringstream std::wostringstream
	#define Tfopen_s _wfopen_s
	#define Tfputs fputws
	#define Tfgets fgetws

#endif


#define _L(x) L ## x
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

#define Log_lf '\n'
#define Log_ts "[INFO]"
#define Log_wr "[WARNING]"
#define Log_er "[ERROR]"

#ifdef _CONSOLE
#define ANSI_RESET "\033[0m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_RED "\033[31m"

#else
#define ANSI_RESET ""
#define ANSI_GREEN ""
#define ANSI_YELLOW ""
#define ANSI_RED ""

#endif


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
		static std::queue<Tstr> LogFileWrite_Queue; //日志WriteConfigFile队列
		static std::thread LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
		static std::atomic<bool> LogFileWriteThreadStop;
		static std::condition_variable cv_LogFileQueue;
#endif

	public:
		Log(bool cmd, bool release)
			: CMD(cmd), Release(release), ShowTime(true), ShowLog(true)
		{
			Init();
		}

		~Log();

	private:
		std::wstring StringToWstring(const std::string& str)
		{
			std::wstring wContext;

			if (str.empty()) {
				return wContext;
			}

#ifdef _WINDOWS
			// Windows 版本
			int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);
			if (len <= 0) {
				throw std::runtime_error("Failed to convert string to wide string.");
			}
			std::unique_ptr<wchar_t[]> buffer(new wchar_t[len + 1]);
			if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), buffer.get(), len) <= 0) {
				throw std::runtime_error("Failed to convert string to wide string.");
			}
			buffer[len] = _T('\0');
			wContext.assign(buffer.get());
#else
			//lgcr("stow: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)", lm::er);
#endif

			return wContext;
		}

		std::wstring StringToWstring(std::string&& str)
		{
			std::wstring wContext;

			if (str.empty()) {
				return wContext;
			}

#ifdef _WINDOWS
			// Windows 版本
			int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);
			if (len <= 0) {
				throw std::runtime_error("Failed to convert string to wide string.");
			}
			std::unique_ptr<wchar_t[]> buffer(new wchar_t[len + 1]);
			if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), buffer.get(), len) <= 0) {
				throw std::runtime_error("Failed to convert string to wide string.");
			}
			buffer[len] = _T('\0');
			wContext.assign(buffer.get());
#else
			//lgcr("stow: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)", lm::er);
#endif

			return wContext;
		}

		std::string WstringToString(const std::wstring& wStr)
		{
			std::string context;

			if (wStr.empty()) {
				return context;
			}

#ifdef _WINDOWS
			// Windows 版本
			int len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
			if (len <= 0) {
				throw std::runtime_error("Failed to convert wide string to string.");
			}
			std::unique_ptr<char[]> buffer(new char[len + 1]);
			if (WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, buffer.get(), len, nullptr, nullptr) <= 0) {
				throw std::runtime_error("Failed to convert wide string to string.");
			}
			buffer[len] = '\0';
			context.assign(buffer.get());
#else
			//lgcr("stow: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)", lm::er);
#endif

			return context;
		}

		std::string WstringToString(std::wstring&& wStr)
		{
			std::string context;

			if (wStr.empty()) {
				return context;
			}

#ifdef _WINDOWS
			// Windows 版本
			int len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
			if (len <= 0) {
				throw std::runtime_error("Failed to convert wide string to string.");
			}
			std::unique_ptr<char[]> buffer(new char[len + 1]);
			if (WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, buffer.get(), len, nullptr, nullptr) <= 0) {
				throw std::runtime_error("Failed to convert wide string to string.");
			}
			buffer[len] = '\0';
			context.assign(buffer.get());
#else
			//lgcr("stow: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)", lm::er);
#endif

			return context;
		}
#define stow StringToWstring
#define wtos WstringToString

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

					// 使用Config进行格式化
					Tcout.imbue(std::locale()); // 使用全局Config
					Tcerr.imbue(std::locale());

					//Tcout << "Log Config: zh_CN.UTF-8 完成!\n";

#ifdef _WINDOWS
					//Windows 控制台编码修改: UTF-8

					SetConsoleOutputCP(CP_UTF8);
					SetConsoleCP(CP_UTF8);

#ifndef _CONSOLE
					//分离控制台
					if (FreeConsole() == 0)
					{
						MessageBoxW(0, stow("log: 分离控制台失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
						MessageBoxW(0, stow("错误代码: " + Tto_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					}
					else {
						Tcout << "Log: 分离控制台\n";
					}


					//分配控制台: 当不是控制台程序时
					if (AllocConsole() == 0)
					{
						MessageBoxW(0, stow("log: 分配控制台失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
						MessageBoxW(0, stow("错误代码: " + Tto_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					}
					else {
						Tcout << "Log: 分配控制台\n";
					}

					// 获取标准输出流的句柄
					FILE* FilePtr;
					// 重定向标准输出流
					freopen_s(&FilePtr, "CONOUT$", "w", stdout);
					if (FilePtr == nullptr) {
						MessageBoxW(0, stow("日志: 重定向标准输出流失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
						MessageBoxW(0, stow("错误代码: " + std::to_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
						return;
					}
					// 重定向标准错误流
					freopen_s(&FilePtr, "CONOUT$", "w", stderr);
					if (FilePtr == nullptr) {
						MessageBoxW(0, stow("日志: 重定向标准错误流失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
						MessageBoxW(0, stow("错误代码: " + std::to_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
						return;
					}

					SetConsoleOutputCP(CP_UTF8);
					SetConsoleCP(CP_UTF8);
#endif

					// 获取控制台窗口的句柄
					hConsole = GetConsoleWindow();
#endif
					if (LogFileWrite) {
						//Tcerr << "Log 日志WriteConfigFile: 开始\n";

						//获取 当前路径/Log/Log文件名.txt 
						//创建文件夹 ./Log  .
						Tstr Log_FolderName = (Tstr)"." + PATH_BACKSLASH + "Log";
						if (CreateFolder(Log_FolderName)) {
						}
						else {
							Tcerr << ANSI_YELLOW
								<< "Log: 文件夹[" + Log_FolderName + "]创建失败!\n"
								<< ANSI_RESET;

							Log_FolderName = (Tstr)".";
						}

						//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
						Tstr Log_FileName = GetFormattingTime("%Y-%m-%d_%H-%M-%S", "", "") + ".txt";
						// ./Log/时间.txt  ||  ./时间.txt
						Tstr Log_FilePath = Log_FolderName + PATH_BACKSLASH + Log_FileName; //总是添加 /
																					
						try {
							//打开文件
							Tstr _文件编码 = "UTF-8";
							//Tcerr << "Log: 日志输出文件名 " << Log_FilePath << "\n" << std::endl;
							LogFileStream_Out = std::move(std::ofstream(Log_FilePath, std::ios::out));
							if (!LogFileStream_Out) {
								Tcerr << ANSI_RED
									<< (Tstr)"Log: " + Log_FilePath + " 打开文件失败!\n"
									<< ANSI_RESET;
								return;
							}
							Tcerr << ANSI_GREEN
								<< (Tstr)"Log: " + Log_FilePath + " 打开文件成功!\n"
								<< ANSI_RESET;
							//Tcerr << "Log: 文件 开始写入!\n";

							if (_文件编码 == "UTF-8BOM") {
								// 写入 UTF-8 BOM (EF BB BF)
								Tcerr << "Log: 写入 UTF-8BOM 编码(EF BB BF)\n";

								LogFileStream_Out.put(0xEF);
								LogFileStream_Out.put(0xBB);
								LogFileStream_Out.put(0xBF);
							}
						}
						catch (...) {
							Tcerr << "Typical_Tool::Log::Init()::LogFileStream_Out\n";
						}

#ifdef _Thread
						//初始化: 日志WriteConfigFile线程
						try {
							LogFileProcessing = std::thread(&Log::LogWirteToFile,
								this);
							LogFileProcessing.detach(); //分离线程
							Uerr << "Log: 日志写入线程 分离到后台\n";
						}
						catch (...) {
							Tcerr << "Typical_Tool::Log::Init()::LogFileProcessing\n";
						}
#endif
					}

					//完成初始化
					FirstInit = true;
				}
			}
			catch (...) {
				Tcerr << "void Typical_Tool::Log::Init()\n";
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
				Tcerr << "Typical_Tool::Log::LogWriteToFile() - " << e.what() << "\n";
			}
			catch (...) {
				Tcerr << "Typical_Tool::Log::LogWirteToFile()\n";
			}
#endif
		}

		template<class T = bool>
		void StopLogWrite()
		{
#ifdef _Thread
			LogFileWriteThreadStop.store(true);
			cv_LogFileQueue.notify_one(); // 通知线程退出
			Tcout << "Log: 日志写入线程 退出\n" << std::endl;
#endif
		}

	private:

		template<class T = bool>
		void SetConsoleTextColor(const Tstr& _Color)
		{
			Tcout << _Color; //修改颜色
		}
		template<class T = bool>
		void SetConsoleTextColor_Error(const Tstr& _Color)
		{
			Tcerr << _Color; //修改颜色
		}
		template<class T = bool>
		void ReSetConsoleTextColor()
		{
			Tcout << ANSI_RESET; //还原颜色
		}
		template<class T = bool>
		void ReSetConsoleTextColor_Error()
		{
			Tcerr << ANSI_RESET; //还原颜色
		}

		template<class T = bool>
		void ConsoleOutput(const Tstr& _Test)
		{
			Tcout << _Test;
		}
		template<class T = bool>
		void ConsoleOutput_Error(const Tstr& _Test)
		{
			Tcerr << _Test;
		}


		// _WINDOWS || _CONSOLE
		template<class T = bool>
		void Logs_ustr_ustr(const Tstr& text, const Tstr& title)
		{
			if (CMD)
			{
				Tstr temp;
				/*if (ShowTime) {
					Tcout << GetFormattingTime();
				}*/
				temp = title + ": " + text;
				ConsoleOutput(temp);

				//WriteConfigFile log日志
				if (LogFileWrite) {
#ifdef _Thread
					{
						//锁 日志WriteConfigFile队列
						std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

						if (LogAllOutput) {
							LogFileWrite_Queue.push(temp);
						}
					}
					cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
					if (LogAllOutput) {
						if (ShowTime) {
							LogFileStream_Out << GetFormattingTime();
						}
						LogFileStream_Out << temp;
					}
				}

				return;
			}
#ifdef _WINDOWS
#ifndef UNICODE
			MessageBoxW(NULL, stow(text).c_str(), stow(title).c_str(), MB_OK);
#else
			MessageBoxW(NULL, text.c_str(), title.c_str(), MB_OK);
#endif
#endif
		}
	
		template<class T = bool>
		void Logs_ustr(const Tstr& text, const LogMessage& lm)
		{
			switch (lm)
			{
			case LogMessage::ts:
			{
				if (CMD)
				{
					Tstr temp;
					SetConsoleTextColor(ANSI_GREEN); //在时间输出之前
					/*if (ShowTime) {
						Tcout << GetFormattingTime();
					}*/
					temp = (Tstr)Log_ts + " " + text + "\n";
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//WriteConfigFile log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (LogAllOutput) {
							if (ShowTime) {
								LogFileStream_Out << GetFormattingTime();
							}
							LogFileStream_Out << temp;
						}
					}
					break;
				}
#ifdef _WINDOWS
#ifndef UNICODE
				MessageBoxW(NULL, stow(text).c_str(), stow(Log_ts).c_str(), MB_OK);
#else
				MessageBoxW(NULL, text.c_str(), Log_ts, MB_OK);
#endif
#endif

				break;
			}
			case LogMessage::wr:
			{
				if (CMD)
				{
					Tstr temp;
					SetConsoleTextColor(ANSI_YELLOW); //在时间输出之前
					/*if (ShowTime) {
						Tcout << GetFormattingTime();
					}*/
					temp = (Tstr)Log_wr + " " + text + "\n";
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//WriteConfigFile log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (LogAllOutput) {
							if (ShowTime) {
								LogFileStream_Out << GetFormattingTime();
							}
							LogFileStream_Out << temp;
						}
					}
					break;
				}
#ifdef _WINDOWS
#ifndef UNICODE
				MessageBoxW(NULL, stow(text).c_str(), stow(Log_wr).c_str(), MB_ICONWARNING);
#else
				MessageBoxW(NULL, text.c_str(), Log_wr, MB_ICONWARNING);
#endif
#endif

				break;
			}
			case LogMessage::er:
			{
				if (CMD)
				{
					Tstr temp;
					SetConsoleTextColor_Error(ANSI_RED); //在时间输出之前
					/*if (ShowTime) {
						Tcout << GetFormattingTime();
					}*/
					temp = (Tstr)Log_er + " " + text + "\n";
					ConsoleOutput_Error(temp);
					ReSetConsoleTextColor_Error();

					//WriteConfigFile log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
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
#ifndef UNICODE
				MessageBoxW(NULL, stow(text).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
#else
				MessageBoxW(NULL, text.c_str(), Log_er, MB_ICONSTOP);
#endif
#endif

				break;
			}
			case LogMessage::tx:
			{
				if (CMD)
				{
					/*if (ShowTime) {
						Tcout << GetFormattingTime();
					}*/
					ConsoleOutput(text + "\n");

					//WriteConfigFile log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(text);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (LogAllOutput) {
							if (ShowTime) {
								LogFileStream_Out << GetFormattingTime();
							}
							LogFileStream_Out << text;
						}
					}
					break;
				}
#ifdef _WINDOWS
#ifndef UNICODE
				MessageBoxW(NULL, stow(text).c_str(), stow("Text").c_str(), MB_OK);
#else
				MessageBoxW(NULL, text.c_str(), _T("Text"), MB_OK);
#endif
#endif
				
				break;
			}
			case LogMessage::lf: {
				if (CMD)
				{
					ConsoleOutput(text);

					//WriteConfigFile log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(text);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (LogAllOutput) {
							if (ShowTime) {
								LogFileStream_Out << GetFormattingTime();
							}
							LogFileStream_Out << text;
						}
					}
					return;
				}
			}
			}
		}
		template<class T = bool>
		void Logs_lgm(const LogMessage& lm)
		{
			if (lm == LogMessage::lf)
			{
				if (CMD)
				{
					Tstr temp = "\n";
					ConsoleOutput(temp);

					//WriteConfigFile log日志
					if (LogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							std::lock_guard<std::mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (LogAllOutput) {
								LogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (LogAllOutput) {
							if (ShowTime) {
								LogFileStream_Out << GetFormattingTime();
							}
							LogFileStream_Out << temp;
						}
					}
					return;
				}
			}
		}


		// _DEBUG || NDEBUG
		template<class T = bool>
		void Logs(const Tstr& text, const Tstr& title)
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
		void Logs(const Tstr& text, const LogMessage& lm)
		{
#ifdef _DEBUG
			Logs_ustr(text, lm);
			return;
#endif
			if (Release)
			{
				Logs_ustr(text, lm);
			}
		}
		template<class T = bool>
		void Logs(const LogMessage& lm)
		{
#ifdef _DEBUG
			Logs_lgm(lm);
			return;
#endif
			if (Release)
			{
				Logs_lgm(lm);
			}
		}

	public:

		//显示/隐藏 Log消息
		template<class T = bool>
		void operator()(const Tstr& text, const Tstr& title)
		{
			if (ShowLog)
			{
				Logs(text, title);
			}
		}
		template<class T = bool>
		void operator()(const Tstr& text, const LogMessage& lm = lm::tx)
		{
			if (ShowLog)
			{
				Logs(text, lm);
			}
		}
		template<class T = bool>
		void operator()(Tstr&& text, const LogMessage& lm = lm::tx)
		{
			if (ShowLog)
			{
				Logs(text, lm);
			}
		}
		template<class T = bool>
		void operator()()
		{
			if (ShowLog)
			{
				Logs(lm::lf);
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
			if (attributes == INVALID_FILE_ATTRIBUTES)
			{
				// 路径不存在或出错，尝试创建目录  
				if (CreateDirectory(folderPath.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
				{
					Tcout << "Log: 文件夹[" + folderPath + "]创建成功!" + "\n";
					// 创建成功
					return true;
				}
				Tcerr << "Log: 文件夹[" + folderPath + "]创建失败!" + "\n";
				// 创建失败且不是因为路径已存在  
				return false;
			}
			else if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				Tcout << "Log: 文件夹[" + folderPath + "]已存在" + "\n";
				// 路径已经是一个目录  
				return true;
			}
			// 路径存在但不是目录（可能是一个文件）  
			Tcout << "Log: 文件夹[" + folderPath + "]创建失败(路径存在, 但不是目录)!" + "\n";
#else
			Tcout << "Log: 文件夹[" + folderPath + "]创建失败(#ifndef _WINDOWS)!" + "\n";
#endif
			return false;
		}


	public:

		void SetShowLog(bool showLog);
#define 设置日志显示 SetShowLog
		void SetShowTime(bool showTime);
#define 设置时间显示 SetShowTime

		// _WINDOWS: Win32 API
		void SetShowConsole(bool showConsole);
#define 设置控制台显示 SetShowConsole


	private:
		template<class Temp = bool>
		Tstr GetFormattingTime(const Tstr& timeFormat = "%Y-%m-%d %H:%M:%S",
			const Tstr& textLeftFormat = "[", const Tstr& textRigthFormat = "]")
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();;
			// 获取当前时间点（自epoch以来的时间）
			// 将时间点转换为time_t（用于localtime函数）
			std::time_t tm = std::chrono::system_clock::to_time_t(now);
			// 使用localtime函数将time_t转换为本地时间（std::tm结构）
			std::tm* now_tm = std::localtime(&tm);

			// 使用 std::put_time 格式化时间
			Tostringstream oss;
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
		* !=3: All(lm: ts/wr/tx) log level output
		*/
		template<class T = bool>
		static void SetAllLogFileWrite(bool logFileWrite, int logLevel = lm::er)
		{
			LogFileWrite = logFileWrite;

			//输出所有级别
			if (logLevel != lm::er) {
				LogAllOutput = true;

				Tcout << ANSI_YELLOW 
					<< "Log: File Output level >> All <<"
					<< ANSI_RESET << std::endl;
			}
			else {
				LogAllOutput = false;

				Tcout << ANSI_YELLOW
					<< "Log: File Output level >> Error <<"
					<< ANSI_RESET << std::endl;
			}

			Tcout << std::endl;
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
		lgcr("\n_WINDOWS");
		lgcr();
		lgcr("\nTypical_Tool::README()");
		lgcr("\n 支持 Unicode: #define UNICODE");
		lgcr("\n 使用方法-函数对象, 代码如下:");
		lgcr("\n 输出[_CONSOLE | _WINDOWS](_DEBUG)", "lg()");
		lgcr("\n 输出[_CONSOLE | _WINDOWS](_DEBUG | Release)", "lgr()");
		lgcr("\n CMD输出[_WINDOWS](_DEBUG)", "lgc()");
		lgcr("\n CMD输出[_WINDOWS](_DEBUG | Release)", "lgcr()");
		lgcr();
		lgcr("lm", "enum");
		lgcr("\t#ifndef _English");
		lgcr("\tlgm::tx,  //文本: 只输出文本");
		lgcr("\tlgm::ts,  //提示");
		lgcr("\tlgm::wr,  //警告");
		lgcr("\tlgm::er   //错误");
		lgcr();
		lgcr("\n日志显示开关");
		lgcr("\tTypical_Tool::设置日志显示(bool)", );
		lgcr("\tTypical_Tool::设置时间显示(bool)", );
		lgcr("\tTypical_Tool::Log::设置日志显示(bool)");
		lgcr("\tTypical_Tool::Log::设置时间显示(bool)");
		lgcr();
		lgcr();
		lgcr();
		lgcr();
		lgcr("\nTypical_Tool::README()", );
		lgcr("\n Unicode support: #define UNICODE", );
		lgcr("\n Using method - function object, the code is as follows:");
		lgcr("\n Print [_CONSOLE | _WINDOWS](_DEBUG)", "lg()");
		lgcr("\n Print [_CONSOLE | _WINDOWS](_DEBUG | Release)", "lgr()");
		lgcr("\n Command Print [_WINDOWS](_DEBUG)", "lgc()");
		lgcr("\n Command Print [_WINDOWS](_DEBUG | Release)", "lgcr()");
		lgcr();
		lgcr();
		lgcr("lm", "enum");
		lgcr("\t#ifdef _English");
		lgcr("\tlgm::ts,  //Tips");
		lgcr("\tlgm::wr,  //Warning");
		lgcr("\tlgm::er   //Error");
		lgcr();
		lgcr("\nLog display switch:");
		lgcr("\tTypical_Tool::SetShowLog(bool)");
		lgcr("\tTypical_Tool::SetConsoleTimeShow(bool)");
		lgcr("\tTypical_Tool::Log::SetShowLog(bool)");
		lgcr("\tTypical_Tool::Log::SetConsoleTimeShow(bool)");
	}
}


#endif
