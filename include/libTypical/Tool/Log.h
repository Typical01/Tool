#pragma once

#ifndef _LOG_H
#define _LOG_H


#include <libTypical/Tool/pch.h>
#include <libTypical/Tool/Tchar_Typical.h>
#include <libTypical/Tool/StringManage.h>


namespace Typical_Tool
{
	using namespace StringManage;


	class LogStringBuffer {
	public:
		Tstr AnsiEsc_Color;
		Tstr TimeStr;
		Tstr _Text;
		Tstr AnsiEsc_Reset;

	public:
		LogStringBuffer()
			: AnsiEsc_Color(TEXT("")), TimeStr(TEXT("")), _Text(TEXT("")), AnsiEsc_Reset(TEXT("")) { }
		LogStringBuffer(const Tstr& AnsiEsc_Color, const Tstr& _Text, const Tstr& AnsiEsc_Reset)
			: AnsiEsc_Color(AnsiEsc_Color), TimeStr(TEXT("")), _Text(_Text), AnsiEsc_Reset(AnsiEsc_Reset) { }
		LogStringBuffer(const Tstr& AnsiEsc_Color, const Tstr& TimeStr, const Tstr& _Text, const Tstr& AnsiEsc_Reset)
			: AnsiEsc_Color(AnsiEsc_Color), TimeStr(TimeStr), _Text(_Text), AnsiEsc_Reset(AnsiEsc_Reset) { }
	};
	template<class T = bool>
	Tostream& operator<<(Tostream& Ostream, const LogStringBuffer& LogStringBuffer) {
		return Ostream << LogStringBuffer.AnsiEsc_Color << LogStringBuffer.TimeStr 
			<< LogStringBuffer._Text << LogStringBuffer.AnsiEsc_Reset;
	}



	class Printf {
	private:
		Tstr FormatStr;

	public:
		// 使用一个占位符（%s）来替换格式化字符串中的参数
		template<typename... Args>
		Printf(const Tstr& format, Args... args) 
		{
			std::vector<Tstr> placeholders = { format };
			this->FormatStr = format_impl(placeholders, args...);
		}

	private:
		// 基础模板：处理最后一个参数
		static Tstr format_impl(std::vector<Tstr>& placeholders) { return placeholders[0]; }
		// 递归模板：处理其他参数
		template<typename T, typename... Args>
		static Tstr format_impl(std::vector<Tstr>& placeholders, T first, Args... args) 
		{
			Tstr formatted_string = placeholders[0];
			size_t pos = formatted_string.find(TEXT("%s")); // 查找第一个占位符

			if (pos != Tstr::npos) {
				Tostringstream oss;
				oss << first;  // 将第一个参数转换为字符串
				formatted_string.replace(pos, 2, oss.str()); // 替换占位符
			}
			placeholders[0] = formatted_string; // 递归替换剩余的占位符
			return format_impl(placeholders, args...);
		}

	public:
		// 获取格式化后的字符串
		Tstr str() const { return this->FormatStr; }
		// 自动转换
		operator Tstr() const { return str(); }
	};
	

	enum class LogMessage {
		Tips,		//提示: Tips [INFO] | 绿色
		Warning,	//警告: Warning [WARRING] | 黄色
		Error,		//错误: Error [ERROR] | 红色
		End,		//结束: 不换行
		LineFeed	//换行: '\n'
	};
	constexpr LogMessage Tip() { return LogMessage::Tips; }
	constexpr LogMessage War() { return LogMessage::Warning; }
	constexpr LogMessage Err() { return LogMessage::Error; }
	constexpr LogMessage End() { return LogMessage::End; }
	constexpr LogMessage Lnf() { return LogMessage::LineFeed; }


#ifndef MB_OK
#define MB_OK 0
#endif
#ifndef MB_ICONSTOP
#define MB_ICONSTOP 0
#endif
#ifndef MB_ICONWARNING
#define MB_ICONWARNING 0
#endif


	/* 
	* 不需要控制台颜色显示: #undef _ANSIESC_CONSOLE_CHAR_COLOR
	* Debug: 需要显示指定 | 显示 Log中的 Debug消息
	* WIN32APP:	对于 Win32app 显示指定控制台分离
	* English: 标准库控制台的地区支持, 默认为中文
	*/
	class Log {
	private:
#ifdef _WINDOWS
		inline static HWND hConsole = nullptr; //控制台句柄
#endif
		inline static std::shared_ptr<Tofstream> LogFileStream_Out;			//日志文件流 输出
		inline static std::mutex Mutex_LogFileStream_Out;
		inline static std::queue<Tstr> LogFileWrite_Queue;					//日志 WriteConfigFile队列
		inline static std::thread LogFileProcessing;						//日志文件处理: 主要是输出到[./Log/时间_程序名.txt]文件
		inline static std::atomic<bool> IsLogFileWriteThreadStop = true;	//是否 停止日志文件写入线程

#ifndef UNICODE
		inline static Tostream& Tout = std::cout;
		inline static Tostream& Terr = std::cerr;
			   
#else		   
#ifdef UTF8	   
		inline static Tostream& Tout = std::cout;
		inline static Tostream& Terr = std::cerr;
#else		   
		inline static Tostream& Tout = std::wcout;
		inline static Tostream& Terr = std::wcerr;
#endif

#endif

	private:
		inline static bool LogInitialize = false;			//控制台初始化
		inline static bool IsLogFileWrite = false;			//是否 写入日志文件
		inline static bool IsLogAllOutput = false;			//是否 输出所有日志
		inline static bool bShowLog = true;					//显示日志
		inline static bool bShowTime = true;				//显示时间
		inline static bool Debug = false;					//#define _Debug(或自定义的Debug) (Debug == true)

		bool Console;		//控制台
		bool Release;	//发行版

	public:
		Log(bool Console, bool Release)
			: Console(Console), Release(Release)
		{
			Initialize();
		}

		~Log()
		{
			StopLogWrite(); //当程序结束时, 退出 日志文件写入线程
		}

	public:
		/* 控制台初始化状态: false(需要初始化), true(跳过初始化)
		*/
		void Initialize() {
			//控制台初始化
			if (!LogInitialize) {
#ifdef WIN32APP
#ifdef _WINDOWS
				//Windows 控制台编码修改: UTF-8
				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);
#ifndef _CONSOLE
				//分离控制台
				if (FreeConsole() == 0) {
					LogMessageBox(TEXT("Log::Initialize: 分离控制台失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), ToStr(GetLastError())).str().c_str(), Log_Error, MB_ICONSTOP);
				}
				//分配控制台: 当不是控制台程序时
				if (AllocConsole() == 0) {
					LogMessageBox(TEXT("Log::Initialize: 分配控制台失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), ToStr(GetLastError())).str().c_str(), Log_Error, MB_ICONSTOP);
				}

				// 获取标准输出流的句柄
				FILE* FilePtr;
				// 重定向标准输出流
				errno_t err = freopen_s(&FilePtr, "CONOUT$", "w", stdout);
				if (err != 0) {
					LogMessageBox(TEXT("Log::Initialize: 重定向标准输出流失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), err), Log_Error, MB_ICONSTOP);
					return;
				}
				// 重定向标准错误流
				err = freopen_s(&FilePtr, "CONOUT$", "w", stderr);
				if (err != 0) {
					LogMessageBox(TEXT("Log::Initialize: : 重定向标准错误流失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), err), Log_Error, MB_ICONSTOP);
					return;
				}
				err = freopen_s(&FilePtr, "CONIN$", "r", stdin);
				if (err != 0) {
					LogMessageBox(TEXT("Log::Initialize: : 重定向标准输入流失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), err), Log_Error, MB_ICONSTOP);
					return;
				}
				// 设置标准句柄
				HANDLE hConOut = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE | GENERIC_READ,
					FILE_SHARE_WRITE | FILE_SHARE_READ,
					NULL, OPEN_EXISTING, 0, NULL);
				SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
				SetStdHandle(STD_ERROR_HANDLE, hConOut);

				HANDLE hConIn = CreateFile(TEXT("CONIN$"), GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, 0, NULL);
				SetStdHandle(STD_INPUT_HANDLE, hConIn);
#endif

				// 获取控制台窗口的句柄
				hConsole = GetConsoleWindow();
				EnableAnsiEscape();

				//检查标准输出流
				if (!Tout.good() || !Terr.good()) {
					LogMessageBox(TEXT("Log::Initialize: Tout/Terr 无效!"), Log_Error, MB_ICONSTOP);
				}
				else {
					// 设置全局地区: 数字表示方法...
#ifdef English
					std::locale::global(std::locale("en_US.UTF-8"));
					Log_Out(ANSIESC_GREEN, Terr, Printf(TEXT("Log::Initialize: en_US.UTF-8 Successful!\n"), Log_Tips), ANSIESC_RESET, -1, true);
#else
					std::locale::global(std::locale("zh_CN.UTF-8"));
					Log_Out(ANSIESC_GREEN, Terr, Printf(TEXT("Log::Initialize: zh_CN.UTF-8 成功!\n"), Log_Tips), ANSIESC_RESET, -1, true);
#endif
					// 设置 cout/cerr 使用全局地区
					Tout.imbue(std::locale(""));
					Terr.imbue(std::locale(""));
				}
#endif
#else
				//检查标准输出流
				if (!Tout.good()) {
					LogMessageBox(TEXT("Log::Initialize: Tout 无效!"), Log_Error, MB_ICONSTOP);
					throw Truntime_error(TEXT("Log::Initialize: Tout Not good!"));
				}
				if (!Terr.good()) {
					LogMessageBox(TEXT("Log::Initialize: Terr 无效!"), Log_Error, MB_ICONSTOP);
					throw Truntime_error(TEXT("Log::Initialize: Terr Not good!"));
				}

				// 设置全局地区: 数字表示方法...
#ifdef English
				std::locale::global(std::locale("en_US.UTF-8"));
				Log_Out(ANSIESC_GREEN, Terr, Printf(TEXT("Log::Initialize: en_US.UTF-8 Successful!\n"), Log_Tips), ANSIESC_RESET, -1, true);
#else
				std::locale::global(std::locale("zh_CN.UTF-8"));
				Log_Out(ANSIESC_GREEN, Terr, Printf(TEXT("Log::Initialize: zh_CN.UTF-8 成功!\n"), Log_Tips), ANSIESC_RESET, -1, true);
#endif
				// 设置 cout/cerr 使用全局地区
				Tout.imbue(std::locale(""));
				Terr.imbue(std::locale(""));
#endif


				//完成初始化
				LogInitialize = true;
			}
		}


	public:
		static void LogWirte(const LogStringBuffer& _LogBuffer) {
			if (IsLogFileWrite) { //写入文件
				std::lock_guard<std::mutex> QueueGuard(Mutex_LogFileStream_Out);
				LogFileWrite_Queue.push(_LogBuffer.TimeStr);
				LogFileWrite_Queue.push(_LogBuffer._Text);
			}
		}

	private:
		void LogWirteToFile(std::shared_ptr<Tofstream> _LogFileStream_Out, const Tstr& _Log_FilePath) {
			try {
				// 不退出, 且队列不为空
				while (1) {
					bool bSleep = false;
					{
						std::lock_guard<std::mutex> QueueGuard(Mutex_LogFileStream_Out);

						//写入日志
						if (!LogFileWrite_Queue.empty()) {
							if (_LogFileStream_Out->good()) {
								*_LogFileStream_Out << LogFileWrite_Queue.front();
								LogFileWrite_Queue.pop();
							}
							else { //日志文件写入流出错
								_LogFileStream_Out->open(_Log_FilePath, std::ios::app);
							}
						}
						else { //没有剩余日志写入
							if (_LogFileStream_Out->good()) {
								_LogFileStream_Out->close();
							}
							else { //文件流关闭后休眠, 减少cpu占用
								bSleep = true;
							}

							if (IsLogFileWriteThreadStop.load()) { //停止写入线程
								if (_LogFileStream_Out->good()) {
									_LogFileStream_Out->close();
								}
								break;
							}
						}
					}

					if (bSleep) {
						Sleep(50, false);
						bSleep = false;
					}
				}
				if (Debug) {
					Log_Out(ANSIESC_YELLOW, Terr, Printf(TEXT("%sLog::LogWirteToFile: 日志写入线程: 退出!\n"), Log_Warning), ANSIESC_RESET, -1, true);
				}
			}
			catch (const std::exception& e) {
				Log_Out(ANSIESC_RED, Terr, Printf(TEXT("%sLog::LogWirteToFile: 日志写入线程: 异常[%s]!\n"), Log_Warning, u8tow(e.what())), ANSIESC_RESET, -1, true);
			}
			catch (...) {
				Log_Out(ANSIESC_RED, Terr, Printf(TEXT("%sLog::LogWirteToFile: 日志写入线程: 未知异常!\n"), Log_Warning), ANSIESC_RESET, -1, true);
			}
		}
		static void StopLogWrite() {
			if (!IsLogFileWriteThreadStop.load()) {
				IsLogFileWriteThreadStop.store(true); //退出 条件
				LogFileProcessing.join(); //主线程等待 后台退出
				Log_Out(ANSIESC_YELLOW, Terr, Printf(TEXT("%sLog::StopLogWrite: 日志写入完成! 程序退出...\n"), Log_Warning), ANSIESC_RESET, -1, true);
				//Time::wait_s<time::ms>(10); //主线程等待 后台退出
			}
		}

	private:
		static void Log_Out(const Tstr& _ANSIESC_Color, Tostream& _Ostream, const Tstr& _str, const Tstr& _ANSIESC_RESET, const int& _MessageKey = -1, bool _IsWirteFile = false) {
			if (!_Ostream.good()) {
				return;
			}
			
			LogStringBuffer tempLogBuffer;
			if (bShowTime) {
				tempLogBuffer = { _ANSIESC_Color, Log::GetFormatTime(), _str, _ANSIESC_RESET };
			}
			else {
				tempLogBuffer = { _ANSIESC_Color, _str, _ANSIESC_RESET };
			}
			_Ostream << tempLogBuffer;
			//WriteConfigFile log日志
			if (IsLogAllOutput) { //所有级别
				LogWirte(tempLogBuffer);
			}
			else { //指定级别
				if (_IsWirteFile) {
					LogWirte(tempLogBuffer);
				}
			}
		}

		// _WINDOWS || _CONSOLE
		void Logs_ustr(const Tstr& _Text, const LogMessage& _Lm, const int& _MessageKey) {
			switch (_Lm) {
			case LogMessage::Tips: {
				if (this->Console) {
					Log_Out(ANSIESC_GREEN, Tout, Printf(TEXT("%s%s%s"), Log_Tips, _Text, Log_LineFeed), ANSIESC_RESET, _MessageKey);
				}
				else {
					LogMessageBox(_Text, Log_Tips, MB_OK);
				}

				break;
			}
			case LogMessage::Warning: {
				if (this->Console) {
					Log_Out(ANSIESC_YELLOW, Tout, Printf(TEXT("%s%s%s"), Log_Warning, _Text, Log_LineFeed), ANSIESC_RESET, _MessageKey);
				}
				else {
					LogMessageBox(_Text, Log_Warning, MB_ICONWARNING);
				}

				break;
			}
			case LogMessage::Error: {
				if (this->Console) {
					Log_Out(ANSIESC_RED, Terr, Printf(TEXT("%s%s%s"), Log_Error, _Text, Log_LineFeed), ANSIESC_RESET, _MessageKey, true);
				}
				else {
					LogMessageBox(_Text, Log_Error, MB_ICONSTOP);
				}

				break;
			}
			case LogMessage::End: {
				if (this->Console) {
					Log_Out(TEXT(""), Terr, Printf(TEXT("%s%s"), Log_Tab, _Text), TEXT(""), _MessageKey);
					return;
				}
				else {
					LogMessageBox(_Text, Log_Text, MB_OK);
				}

				break;
			}
			case LogMessage::LineFeed: {
				if (this->Console) {
					Log_Out(TEXT(""), Terr, Printf(TEXT("%s%s%s"), Log_Tab, _Text, Log_LineFeed), TEXT(""), _MessageKey);
					return;
				}
				else {
					LogMessageBox(_Text, Log_Text, MB_OK);
				}

				break;
			}
			}
		}
		void Logs_lgm()
		{
			if (this->Console) {
				Log_Out(TEXT(""), Tout, Log_LineFeed, TEXT(""), -1);
				return;
			}
		}


	public:

		//显示/隐藏 Log消息
		void operator()(const Tstr& _Text, LogMessage(*_Lm)() = &Lnf, const int& _MessageKey = -1)
		{
			if (!bShowLog) { return; }

			if (Debug) { // Debug
				Logs_ustr(_Text, _Lm(), _MessageKey);
			}
			else { // Release
				if (this->Release) {
					Logs_ustr(_Text, _Lm(), _MessageKey);
				}
			}
		}
		void operator()(LogMessage(*_Lm)(), const Tstr& _Text, const int& _MessageKey = -1)
		{
			if (!bShowLog) { return; }

			if (Debug) { // Debug
				Logs_ustr(_Text, _Lm(), _MessageKey);
			}
			else { // Release
				if (this->Release) {
					Logs_ustr(_Text, _Lm(), _MessageKey);
				}
			}
		}
		/*
		* 输出 _LineNumber 个换行符
		*/
		void operator()(__int64 _LineNumber = 0)
		{
			if (!bShowLog) { return; }

			if (Debug) { // Debug
				for (auto i = 0; i < _LineNumber; i++) {
					Logs_lgm();
				}
			}
			else { // Release
				if (this->Release) {
					for (auto i = 0; i < _LineNumber; i++) {
						Logs_lgm();
					}
				}
			}
		}

	public:
		//设置 日志显示
		static void SetShowLog(bool _ShowLog);
		//设置 时间显示
		static void SetShowTime(bool _ShowTime);
		//设置 控制台显示: _WINDOWS(Win32 API)
		static void SetShowConsole(bool _ShowConsole);
		//设置 Debug 模式
		static void SetDebug(const bool& _Debug);

	public:
		/* 设置 所有日志写入
		* level:
		* Err: Error log level output
		* !=Err: All(_Lm: Tip/War/tx) log level output
		*/

		/// <summary>
		/// 设置 所有日志写入
		/// </summary>
		/// <param name="_LogFileWrite">: 日志文件写入</param>
		/// <param name="_LogFileName">: 日志文件名</param>
		/// <param name="_LogFilePath">: 日志文件路径</param>
		/// <param name="_LogLevel">: 日志文件等级</param>
		/// <param name="bSingleLogFile">: 单日志文件</param>
		void SetAllLogFileWrite(bool _LogFileWrite, const Tstr& _LogFileName, const Tstr& _LogFilePath, int _LogLevel = static_cast<int>(LogMessage::Error), bool bSingleLogFile = true)
		{
			IsLogFileWrite = _LogFileWrite;
			IsLogFileWriteThreadStop = false;

			if (IsLogFileWrite) {
				//获取 当前路径/Log/文件名_Log.txt 
				//创建文件夹 Log
				Tstr Log_FilePath = _LogFileName + TEXT("_Log.txt");
				if (!bSingleLogFile) {
					Tstr Log_FolderName = TEXT("Log");
					if (std::filesystem::exists(Log_FolderName)) { //目录存在
						if (std::filesystem::is_directory(Log_FolderName)) { // 是目录
							//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
							Tstr Log_FileName = Log::GetFormatTime(TEXT("%Y-%m-%d_%H-%M-%S_"), TEXT(""), TEXT(""));
							// ./Log/时间.txt  ||  ./时间.txt
							Log_FilePath = Printf(TEXT("%s%s%s%s"), Log_FolderName, PATH_SLASH, Log_FileName, Log_FilePath);
						}
						else { // 不是目录
						}
					}
					else { //目录不存在
						std::filesystem::create_directory(Log_FolderName); //创建目录
						//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
						Tstr Log_FileName = Log::GetFormatTime(TEXT("%Y-%m-%d_%H-%M-%S_"), TEXT(""), TEXT(""));
						//Log/时间.txt  ||  ./时间.txt
						Log_FilePath = Printf(TEXT("%s%s%s%s"), Log_FolderName, PATH_SLASH, Log_FileName, Log_FilePath);
					}
				}

				//打开文件
				if (Debug) {
					Log_Out(ANSIESC_GREEN, Terr, Printf(TEXT("%sLog::SetAllLogFileWrite: 日志输出文件名[%s]\n"), Log_Tips, Log_FilePath), ANSIESC_RESET, -1, true);
				}
				if (_LogFilePath != TEXT("")) {
					Log_FilePath = Printf(TEXT("%s\\%s"), _LogFilePath, Log_FilePath);
				}
				LogFileStream_Out = std::make_shared<Tofstream>(Printf(TEXT("%s"), Log_FilePath), std::ios::out);
				if (!LogFileStream_Out->good()) {
					if (Debug) {
						Log_Out(ANSIESC_RED, Terr, Printf(TEXT("%sLog::SetAllLogFileWrite: [%s]打开文件失败!\n"), Log_Error, Log_FilePath), ANSIESC_RESET, -1, true);
					}
					return;
				}
				else {
					if (Debug) {
						Log_Out(ANSIESC_GREEN, Terr, Printf(TEXT("%sLog::SetAllLogFileWrite: [%s]打开文件成功!\n"), Log_Tips, Log_FilePath), ANSIESC_RESET, -1, true);
					}

					//Terr << "Log: 文件 开始写入!\n";
					//初始化: 日志WriteConfigFile线程
					std::thread LogFileWriteThread(&Log::LogWirteToFile, this, std::move(LogFileStream_Out), Log_FilePath);
					LogFileProcessing = std::move(LogFileWriteThread);
					if (Debug) {
						Log_Out(ANSIESC_GREEN, Terr, Printf(TEXT("%sLog::SetAllLogFileWrite: 日志写入线程 启动!\n"), Log_Tips), ANSIESC_RESET, -1, true);
					}
				}
			}

			//输出所有级别
			if (_LogLevel != static_cast<int>(LogMessage::Error)) {
				IsLogAllOutput = true;

				if (Debug) {
					Log_Out(ANSIESC_YELLOW, Terr, Printf(TEXT("%sLog::SetAllLogFileWrite: 文件输出级别[All]n"), Log_Warning), ANSIESC_RESET, -1, true);
				}
			}
			else {
				IsLogAllOutput = false;

				if (Debug) {
					Log_Out(ANSIESC_YELLOW, Terr, Printf(TEXT("%sLog::SetAllLogFileWrite: 文件输出级别[Error]\n"), Log_Warning), ANSIESC_RESET, -1, true);
				}
			}
		}

	private:
		static Tstr GetFormatTime(const Tstr& _TimeFormat = TEXT("%Y-%m-%d %H:%M:%S"),
			const Tstr& _TextLeftFormat = TEXT("["), const Tstr& _TextRigthFormat = TEXT("]"))
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();;
			// 获取当前时间点（自epoch以来的时间）
			// 将时间点转换为time_t（用于localtime函数）
			std::time_t ttm = std::chrono::system_clock::to_time_t(now);
			// 使用localtime函数将time_t转换为本地时间（std::tm结构）
			std::tm* now_tm = std::localtime(&ttm);
			// 使用 std::put_time 格式化时间
			Tostringstream oss;
			oss << std::put_time(now_tm, _TimeFormat.c_str()); // 自定义时间格式
			//不需要修饰字符时, 直接返回格式化后的时间文本
			if (_TextLeftFormat == TEXT("") && _TextRigthFormat == TEXT("")) {
				return oss.str();
			}
			else {
				return _TextLeftFormat + oss.str() + _TextRigthFormat;
			}
		}

		static void Sleep(long long _Number, bool _bShowLog)
		{
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, Terr, Printf(TEXT("%sLog::Sleep: [%s]ms\n"), Log_Warning, _Number).str(), ANSIESC_RESET, -1, true);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(_Number));
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, Terr, Printf(TEXT("%sLog::Sleep: 结束\n"), Log_Warning).str(), ANSIESC_RESET, -1, true);
			}
		}

		static void Wait(long long _Number, bool _bShowLog) 
		{
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, Terr, Printf(TEXT("%sLog::Wait: [%s]ms\n"), Log_Warning, _Number).str(), ANSIESC_RESET, -1, true);
			}
			std::chrono::milliseconds timeTarget = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()) +
				std::chrono::milliseconds(_Number);
			while (timeTarget > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())) {}
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, Terr, Printf(TEXT("%sLog::Wait: 结束\n"), Log_Warning).str(), ANSIESC_RESET, -1, true);
			}
		}

	private:
		/*
		* 启用 ANSI转义符支持
		*/
		void EnableAnsiEscape();

		void LogMessageBox(const Tstr& Text, const Tchar* _LogLevel, int MessageBoxIcon = MB_OK)
		{
#ifdef _WINDOWS
#ifdef UNICODE
#ifdef UTF8
			MessageBoxW(NULL, u8tow(Text).c_str(), u8tow(_LogLevel).c_str(), MessageBoxIcon);
#else
			MessageBoxW(NULL, Text.c_str(), _LogLevel, MessageBoxIcon);
#endif
#else
			MessageBox(NULL, Text.c_str(), _LogLevel, MessageBoxIcon);
#endif
#endif
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

	static Log& LogDebug = lgc;
	static Log& LogRelease = lgcr;
}


#endif
