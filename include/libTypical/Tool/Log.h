#pragma once

#include <libTypical/Tool/pch.h>
#include <libTypical/Tool/Tchar_Typical.h>
#include <libTypical/Tool/StringManage.h>
//#include <libTypical/Tool/Time_Typical.h>


#ifndef _LOG_H
#define _LOG_H


//#define _CONSOLE
#define _Thread
//#undef _WINDOWS


namespace Typical_Tool
{
	using namespace std;
	using namespace StringManage;


	class LogStringBuffer {
	public:
		Tstr AnsiEsc_Color;
		Tstr TimeStr;
		Tstr _Text;
		Tstr AnsiEsc_Reset;

	public:
		LogStringBuffer()
			: AnsiEsc_Color(Tx("")), TimeStr(Tx("")), _Text(Tx("")), AnsiEsc_Reset(Tx("")) { }
		LogStringBuffer(const Tstr& AnsiEsc_Color, const Tstr& _Text, const Tstr& AnsiEsc_Reset)
			: AnsiEsc_Color(AnsiEsc_Color), TimeStr(Tx("")), _Text(_Text), AnsiEsc_Reset(AnsiEsc_Reset) { }
		LogStringBuffer(const Tstr& AnsiEsc_Color, const Tstr& TimeStr, const Tstr& _Text, const Tstr& AnsiEsc_Reset)
			: AnsiEsc_Color(AnsiEsc_Color), TimeStr(TimeStr), _Text(_Text), AnsiEsc_Reset(AnsiEsc_Reset) { }
	};
	template<class T = bool>
	Tostream& operator<<(Tostream& Ostream, const LogStringBuffer& LogStringBuffer) {
		return Ostream << LogStringBuffer.AnsiEsc_Color << LogStringBuffer.TimeStr 
			<< LogStringBuffer._Text << LogStringBuffer.AnsiEsc_Reset;
	}



	class Format {
	private:
		Tstr FormatStr;

	public:
		// 使用一个占位符（%）来替换格式化字符串中的参数
		template<typename... Args>
		Format(const Tstr& format, Args... args) {
			std::vector<Tstr> placeholders = { format };
			this->FormatStr = format_impl(placeholders, args...);
		}

	private:
		// 基础模板：处理最后一个参数
		static Tstr format_impl(std::vector<Tstr>& placeholders) {
			return placeholders[0];
		}

		// 递归模板：处理其他参数
		template<typename T, typename... Args>
		static Tstr format_impl(std::vector<Tstr>& placeholders, T first, Args... args) {
			Tstr formatted_string = placeholders[0];
			size_t pos = formatted_string.find(Tx('%')); // 查找第一个占位符

			if (pos != Tstr::npos) {
				Tostringstream oss;
				oss << first;  // 将第一个参数转换为字符串

				// 替换占位符
				formatted_string.replace(pos, 1, oss.str());
			}

			// 递归替换剩余的占位符
			placeholders[0] = formatted_string;
			return format_impl(placeholders, args...);
		}

	public:
		// 获取格式化后的字符串
		Tstr str() const {
			return this->FormatStr;
		}

		// 自动转换
		operator Tstr() const {
			return str();
		}
	};
	


#ifdef _WINDOWS
	inline HWND hConsole; //控制台句柄
#endif

	enum class LogMessage
	{
		Tips,		//提示: Tips [INFO] | 绿色
		Warning,	//警告: Warning [WARRING] | 黄色
		Error,		//错误: Error [ERROR] | 红色
		End,		//结束: 不换行
		LineFeed	//换行: '\n'
	};
	typedef LogMessage _Lm;
	constexpr LogMessage Tip() { return LogMessage::Tips; }
	constexpr LogMessage War() { return LogMessage::Warning; }
	constexpr LogMessage Err() { return LogMessage::Error; }
	constexpr LogMessage End() { return LogMessage::End; }
	constexpr LogMessage Lnf() { return LogMessage::LineFeed; }
	
	/* 
	* 不需要颜色显示: #undef _ANSIESC_CONSOLE_CHAR_COLOR
	* Debug: 需要显示指定 | 显示 Log中的 Debug消息
	* WIN32APP: 对于 Win32app 显示指定控制台分离
	*/
	class Log {
	private:
		inline static std::shared_ptr<Tofstream> LogFileStream_Out;				//日志文件流 输出
		inline static std::mutex Mutex_LogFileStream_Out;
		inline static std::queue<LogStringBuffer> LogFileWrite_Queue;			//日志 WriteConfigFile队列
		inline static std::thread LogFileProcessing;							//日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
		static std::atomic<bool> IsLogFileWriteThreadStop;						//是否 停止日志文件写入线程

		//保存格式
		inline static std::deque<std::pair<int, LogStringBuffer>> ConsoleMessages_Deque;	// 控制台消息 队列
		inline static std::unordered_map<int, size_t> ConsoleMessages_Index;				// _ID 到索引的映射

	private:
		static bool LogInitialize;			//控制台初始化
		static bool IsLogFileWrite;			//是否 写入日志文件
		static bool IsLogAllOutput;			//是否 输出所有日志
		static bool ShowLog;				//显示日志
		static bool ShowTime;				//显示时间
		static bool SingleLogFile;			//单一日志文件
		static bool Debug;					//#define _Debug(或自定义的Debug) (Debug == true)
		static bool bFlushConsoleOutput;	//可刷新的控制台输出
		static long long PauseTime;			//刷新间隔

		bool CMD;		//控制台
		bool Release;	//发行版

	public:
		Log(bool _CMD, bool _Release)
			: CMD(_CMD), Release(_Release)
		{
			Initialize();
		}

		~Log();

	public:
		/* 控制台初始化状态: false(需要初始化), true(跳过初始化)
		*/
		void Initialize() {
			//控制台初始化
			if (!LogInitialize) {
				// 设置全局区域为 "zh_CN.UTF-8"
				locale::global(locale("zh_CN.UTF-8"));
				if (!Tout.good()) {
					throw Truntime_error(_LOGERRORINFO(Tx("Tout Not good!")));
				}
				if (!Terr.good()) {
					throw Truntime_error(_LOGERRORINFO(Tx("Terr Not good!")));
				}
				// 使用Config进行格式化
				Tout.imbue(locale("zh_CN.UTF-8")); // 使用全局Config
				Terr.imbue(locale("zh_CN.UTF-8"));
				Log_Out(ANSIESC_GREEN, Terr, Format(Tx("Log Config: zh_CN.UTF-8 完成!\n"), Log_Tips), ANSIESC_RESET, -1, true);
#ifdef WIN32APP
#ifdef _WINDOWS
				//Windows 控制台编码修改: UTF-8
				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);
#ifndef _CONSOLE
				//分离控制台
				if (FreeConsole() == 0) {
					MessageBox(0, Tx("log: 分离控制台失败!"), Log_Error, MB_ICONSTOP);
					MessageBox(0, Format(Tx("错误代码: [%]"), ToStr(GetLastError())).str().c_str(), Log_Error, MB_ICONSTOP);
				}
				else {
					if (Debug) {
						Log_Out(ANSIESC_GREEN, Terr, Format(Tx("%Log: 分离控制台.\n"), Log_Tips), ANSIESC_RESET, -1, true);
					}
				}
				//分配控制台: 当不是控制台程序时
				if (AllocConsole() == 0) {
					MessageBox(0, Tx("log: 分配控制台失败!"), Log_Error, MB_ICONSTOP);
					MessageBox(0, Format(Tx("错误代码: [%]"), ToStr(GetLastError())).str().c_str(), Log_Error, MB_ICONSTOP);
				}
				else {
					if (Debug) {
						Log_Out(ANSIESC_GREEN, Terr, Format(Tx("%Log: 分配控制台.\n"), Log_Tips), ANSIESC_RESET, -1, true);
					}
				}

				// 获取标准输出流的句柄
				FILE* FilePtr;
				// 重定向标准输出流
				freopen_s(&FilePtr, "CONOUT$", "w", stdout);
				if (FilePtr == nullptr) {
					MessageBox(0, Tx("日志: 重定向标准输出流失败!"), Log_Error, MB_ICONSTOP);
					MessageBox(0, Format(Tx("错误代码: [%]"), ToStr(GetLastError())).str().c_str(), Log_Error, MB_ICONSTOP);
					return;
				}
				// 重定向标准错误流
				freopen_s(&FilePtr, "CONOUT$", "w", stderr);
				if (FilePtr == nullptr) {
					MessageBox(0, Tx("日志: 重定向标准错误流失败!"), Log_Error, MB_ICONSTOP);
					MessageBox(0, Format(Tx("错误代码: [%]"), GetLastError()).str().c_str(), Log_Error, MB_ICONSTOP);
					return;
				}
#endif
				// 获取控制台窗口的句柄
				hConsole = GetConsoleWindow();
				EnableAnsiEscape();
#endif
#endif
				//完成初始化
				LogInitialize = true;
			}
		}


	public:
		static void LogWirte(const LogStringBuffer& _LogBuffer) {
			if (IsLogFileWrite) { //写入文件
				lock_guard<mutex> QueueGuard(Mutex_LogFileStream_Out);
				LogFileWrite_Queue.push(_LogBuffer);
			}
		}
		static void ConsoleClear() {
			Log::wait(PauseTime, false);
			Terr << Tx("\033[2J\033[H");  // ANSI 转义序列：清屏并将光标移至左上角
		}
		static void ShowMessage() {
			ConsoleClear(); //清空控制台消息残留

			for (auto tempIndex = ConsoleMessages_Deque.begin(); tempIndex != ConsoleMessages_Deque.end(); tempIndex++) {
				Tout << tempIndex->second;
			}
			Tout << std::flush; //立即刷新
		}
		// 添加或更新消息
		static void UpdataMessage(int _ID, const LogStringBuffer& _LogBuffer) {
			if (_ID == -1) { // -1时, 不分配_ID
				ConsoleMessages_Deque.push_back({ -1, _LogBuffer });
			}

			if (ConsoleMessages_Index.find(_ID) != ConsoleMessages_Index.end()) { //现有消息
				// 更新现有消息
				size_t index = ConsoleMessages_Index[_ID];
				ConsoleMessages_Deque[index].second = _LogBuffer;
			}
			else { // 新消息
				ConsoleMessages_Deque.push_back({ _ID, _LogBuffer }); //保存新消息的字符串
				ConsoleMessages_Index[_ID] = ConsoleMessages_Deque.size() - 1; //将 新消息绑定到 索引
			}
		}
		static void UpdataCursorMessage(int _Line, const Tstr& _Message) {
			Tout << "\033[s";					// 保存光标位置 
			Tout << "\033[" << _Line << ";1H";	// 移动光标到第 _Line 行，第 1 列
			Tout << "\033[2K";					// 清除当前行内容
			Tout << _Message;					// 输出新的消息
			Tout << "\033[u";					// 恢复光标位置
		}

	private:
		void LogWirteToFile(std::shared_ptr<Tofstream> _LogFileStream_Out, const Tstr& _Log_FilePath) {
			// 不退出, 且队列不为空
			while (1) {
				lock_guard<mutex> QueueGuard(Mutex_LogFileStream_Out);

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
					if (IsLogFileWriteThreadStop.load()) { //停止写入线程
						if (_LogFileStream_Out->good()) {
							_LogFileStream_Out->close();
						}
						break;
					}
				}
			}
			if (Debug) {
				Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log: 日志写入线程->退出!\n"), Log_Warning), ANSIESC_RESET, -1, true);
			}
		}
		static void StopLogWrite() {
			if (!IsLogFileWriteThreadStop.load()) {
				IsLogFileWriteThreadStop.store(true); //退出 条件
				LogFileProcessing.join(); //主线程等待 后台退出
				Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log: 日志写入完成! 程序退出...\n"), Log_Warning), ANSIESC_RESET, -1, true);
				//Time::wait_s<time::ms>(10); //主线程等待 后台退出
			}
		}
	private:
		/*
		* 启用 ANSI转义符支持
		*/
		void EnableAnsiEscape() {
#ifdef _WINDOWS
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD dwMode = 0;
			if (GetConsoleMode(hOut, &dwMode)) {
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(hOut, dwMode);
			}
#endif
		}

	private:
		static Tstr GetFormatTime(const Tstr& _TimeFormat = Tx("%Y-%m-%d %H:%M:%S"),
			const Tstr& _TextLeftFormat = Tx("["), const Tstr& _TextRigthFormat = Tx("]"))
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
			if (_TextLeftFormat == Tx("") && _TextRigthFormat == Tx("")) {
				return oss.str();
			}
			else {
				return _TextLeftFormat + oss.str() + _TextRigthFormat;
			}
		}

		static void wait(long long _Number, bool _bShowLog) {
			Log::bFlushConsoleOutput = false;
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%等待: [%]ms\n"), Log_Warning, _Number).str(), ANSIESC_RESET, -1, true);
			}
			Log::bFlushConsoleOutput = true;
			std::chrono::milliseconds timeTarget = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()) + 
				std::chrono::milliseconds(_Number);
			while (timeTarget > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())) {}
		}

		static void Log_Out(const Tstr& _ANSIESC_Color, Tostream& _Ostream, const Tstr& _str, const Tstr& _ANSIESC_RESET, const int& _MessageKey = -1, bool _IsWirteFile = false) {
			LogStringBuffer tempLogBuffer;
			if (ShowTime) {
				tempLogBuffer = { _ANSIESC_Color, Log::GetFormatTime(), _str, _ANSIESC_RESET };
			}
			else {
				tempLogBuffer = { _ANSIESC_Color, _str, _ANSIESC_RESET };
			}
			if (!bFlushConsoleOutput) {
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
			else {
				//显示常驻消息
				UpdataMessage(_MessageKey, tempLogBuffer);
			}
		}
		// _WINDOWS || _CONSOLE
		void Logs_ustr(const Tstr& _Text, const LogMessage& _Lm, const int& _MessageKey) {
			switch (_Lm) {
			case LogMessage::Tips: {
				if (this->CMD) {
					Log_Out(ANSIESC_GREEN, Tout, Format(Tx("%%%"), Log_Tips, _Text, Log_LineFeed), ANSIESC_RESET, _MessageKey);
				}
				else {
#ifdef _WINDOWS
					MessageBox(NULL, _Text.c_str(), Log_Tips, MB_OK);
#endif
				}

				break;
			}
			case LogMessage::Warning: {
				if (this->CMD) {
					Log_Out(ANSIESC_YELLOW, Tout, Format(Tx("%%%"), Log_Warning, _Text, Log_LineFeed), ANSIESC_RESET, _MessageKey);
				}
				else {
#ifdef _WINDOWS
					MessageBox(NULL, _Text.c_str(), Log_Warning, MB_ICONWARNING);
#endif
				}

				break;
			}
			case LogMessage::Error: {
				if (this->CMD) {
					Log_Out(ANSIESC_RED, Terr, Format(Tx("%%%"), Log_Error, _Text, Log_LineFeed), ANSIESC_RESET, _MessageKey, true);
				}
				else {
#ifdef _WINDOWS
					MessageBox(NULL, _Text.c_str(), Log_Error, MB_ICONSTOP);
#endif
				}

				break;
			}
			case LogMessage::End: {
				if (this->CMD) {
					Log_Out(Tx(""), Terr, Format(Tx("  %"), _Text), Tx(""), _MessageKey);
					return;
				}
				else {
#ifdef _WINDOWS
					MessageBox(NULL, _Text.c_str(), Log_Text, MB_OK);
#endif
				}

				break;
			}
			case LogMessage::LineFeed: {
				if (this->CMD) {
					Log_Out(Tx(""), Terr, Format(Tx("  %%"), _Text, Log_LineFeed), Tx(""), _MessageKey);
					return;
				}
				else {
#ifdef _WINDOWS
					MessageBox(NULL, _Text.c_str(), Log_Text, MB_OK);
#endif
				}

				break;
			}
			}
		}
		void Logs_lgm()
		{
			if (this->CMD) {
				Log_Out(Tx(""), Tout, Log_LineFeed, Tx(""), -1);
				return;
			}
		}


	public:

		//显示/隐藏 Log消息
		void operator()(const Tstr& _Text, LogMessage (*_Lm)() = &Lnf, const int& _MessageKey = -1)
		{
			if (ShowLog) {
				if (Debug) { // Debug
					if (!this->Release) {
						Logs_ustr(_Text, _Lm(), _MessageKey);
					}
				}
				else { // Release
					if (this->Release) {
						Logs_ustr(_Text, _Lm(), _MessageKey);
					}
				}
			}
		}
		void operator()(LogMessage (*_Lm)(), const Tstr& _Text, const int& _MessageKey = -1)
		{
			if (ShowLog) {
				if (Debug) { // Debug
					if (!this->Release) {
						Logs_ustr(_Text, _Lm(), _MessageKey);
					}
				}
				else { // Release
					if (this->Release) {
						Logs_ustr(_Text, _Lm(), _MessageKey);
					}
				}
			}
		}
		/*
		* 输出 _LineNumber 个换行符
		*/
		void operator()(__int64 _LineNumber = 0)
		{
			if (ShowLog) {
				if (Debug) { // Debug
					if (!this->Release) {
						for (auto i = 0; i < _LineNumber; i++) {

							Logs_lgm();
						}
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
		}

	public:

		//设置日志显示
		static void SetShowLog(bool _ShowLog)
		{
			if (Debug) {
				Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log 设置: 显示日志\n"), Log_Warning), ANSIESC_RESET, -1, true);
			}

			ShowLog = _ShowLog;
		}
		//设置时间显示
		static void SetShowTime(bool _ShowTime)
		{
			if (Debug) {
				Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log 设置: 显示时间\n"), Log_Warning), ANSIESC_RESET, -1, true);
			}

			ShowTime = _ShowTime;
		}
		//设置是否为单一日志文件
		static void SetSingleLogFile(bool _SingleLogFile)
		{
			if (Debug) {
				Terr << std::boolalpha;
				Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log 设置: 单一日志文件: %\n"), Log_Warning, _SingleLogFile), ANSIESC_RESET, -1, true);
				Terr << std::noboolalpha;
			}

			SingleLogFile = _SingleLogFile;
		}
		//设置控制台显示: _WINDOWS(Win32 API)
		static void SetShowConsole(bool _ShowConsole)
		{
#ifdef _WINDOWS
			if (Debug) {
				Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log 设置 : 显示控制台\n"), Log_Warning), ANSIESC_RESET, -1, true);
			}
			//显示/隐藏 窗口
			if (_ShowConsole) {
				ShowWindow(hConsole, SW_SHOWDEFAULT);
			}
			else {
				ShowWindow(hConsole, SW_HIDE);
			}
#else
			if (Debug) {
				Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log 设置 : 显示控制台->无效(#ifndef _WINDOWS)\n"), Log_Warning), ANSIESC_RESET, -1, true);
			}
#endif
		}
		static void SetDebug(const bool& _Debug)
		{
			Debug = _Debug;
		}
		//设置 控制台刷新
		static void SetConsoleFlush(const bool& _bFlush)
		{
			bFlushConsoleOutput = _bFlush;
		}
		//设置 控制台刷新间隔
		static void SetPauseTime(const long long& _PauseTime)
		{
			PauseTime = _PauseTime;
		}
	
	public:
		/* 设置所有日志写入
		* level:
		* Err: Error log level output
		* !=Err: All(_Lm: Tip/War/tx) log level output
		*/
		void SetAllLogFileWrite(bool _LogFileWrite, const Tstr& _LogFileName, int _LogLevel = static_cast<int>(_Lm::Error))
		{
			IsLogFileWrite = _LogFileWrite;
			IsLogFileWriteThreadStop = false;

			if (IsLogFileWrite) {
				//Terr << "Log 日志WriteConfigFile: 开始\n";

				//获取 当前路径/Log/Log文件名.txt 
				//创建文件夹 ./Log  .
				Tstr Log_FilePath = _LogFileName + Tx("_Log.txt");
				if (!SingleLogFile) {
					Tstr Log_FolderName = (Tstr)Tx(".") + PATH_SLASH + Tx("Log");
					if (std::filesystem::exists(Log_FolderName)) { //目录存在
						if (std::filesystem::is_directory(Log_FolderName)) { // 是目录
							//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
							Tstr Log_FileName = Log::GetFormatTime(Tx("%Y-%m-%d_%H-%M-%S_"), Tx(""), Tx(""));
							// ./Log/时间.txt  ||  ./时间.txt
							Log_FilePath = Format(Tx("%%%%"), Log_FolderName, PATH_SLASH, Log_FileName, Log_FilePath);
						}
						else { // 不是目录
						}
					}
					else { //目录不存在
						std::filesystem::create_directory(Log_FolderName); //创建目录
						//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
						Tstr Log_FileName = Log::GetFormatTime(Tx("%Y-%m-%d_%H-%M-%S_"), Tx(""), Tx(""));
						// ./Log/时间.txt  ||  ./时间.txt
						Log_FilePath = Format(Tx("%%%%"), Log_FolderName, PATH_SLASH, Log_FileName, Log_FilePath);
					}
				}

				//打开文件
				if (Debug) {
					Log_Out(ANSIESC_GREEN, Terr, Format(Tx("%Log: 日志输出文件名[%]\n"), Log_Tips, Log_FilePath), ANSIESC_RESET, -1, true);
				}
				LogFileStream_Out = std::make_shared<Tofstream>(Log_FilePath, ios::out);
				if (!LogFileStream_Out->good()) {
					if (Debug) {
						Log_Out(ANSIESC_RED, Terr, Format(Tx("%Log: [%]打开文件失败!\n"), Log_Error, Log_FilePath), ANSIESC_RESET, -1, true);
					}
					return;
				}
				else {
					if (Debug) {
						Log_Out(ANSIESC_GREEN, Terr, Format(Tx("%Log: [%]打开文件成功!\n"), Log_Tips, Log_FilePath), ANSIESC_RESET, -1, true);
					}

					//Terr << "Log: 文件 开始写入!\n";
					//初始化: 日志WriteConfigFile线程
					std::thread LogFileWriteThread(&Log::LogWirteToFile, this, std::move(LogFileStream_Out), Log_FilePath);
					LogFileProcessing = std::move(LogFileWriteThread);
					if (Debug) {
						Log_Out(ANSIESC_GREEN, Terr, Format(Tx("%Log: 日志写入线程 启动!\n"), Log_Tips), ANSIESC_RESET, -1, true);
					}
				}
			}

			//输出所有级别
			if (_LogLevel != static_cast<int>(_Lm::Error)) {
				IsLogAllOutput = true;

				if (Debug) {
					Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log: File Output level >> All <<\n"), Log_Warning), ANSIESC_RESET, -1, true);
				}
			}
			else {
				IsLogAllOutput = false;

				if (Debug) {
					Log_Out(ANSIESC_YELLOW, Terr, Format(Tx("%Log: File Output level >> Error <<\n"), Log_Warning), ANSIESC_RESET, -1, true);
				}
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

	template<class T = bool>
	void Log_README()
	{
		Tout << Tx("\t\tLog_README():")
			<< Tx("#define _ANSIESC_CONSOLE_CHAR_COLOR: 控制台转义字符修改字符颜色\n\n")
			<< Tx("#define _Debug(或自定义 Debug宏): 显示Log中的 Debug消息\n\n")
			<< Tx("#ifndef _DEBUG: _WINDOWS/_CONSOLE\n")
			<< Tx("lgr(Format(Tx(\"%s :没有文件!\"), Path), War); \n")
			<< Tx("_DEBUG: _WINDOWS / _CONSOLE\n")
			<< Tx("lg(\"关闭窗口\", Tip); \n\n")
			<< Tx("#ifndef _DEBUG: _CONSOLE\n")
			<< Tx("lgcr(\"Code: 404\", Err); \n")
			<< Tx("_DEBUG: _CONSOLE\n")
			<< Tx("lgc(\"Log.h\", Lnf); \n");
	}

	static Log& log_Namespace = lgc;



#ifdef _WINDOWS
	class ConsoleDoubleBuffer {
	private:
		HANDLE hStdout;        // 默认标准输出缓冲区
		HANDLE hBuffer[2];     // 双缓冲的两个缓冲区
		int currentBuffer;     // 当前缓冲区索引
		COORD bufferSize;      // 缓冲区大小

	public:
		ConsoleDoubleBuffer() {
			//创建双缓冲
			hStdout = GetStdHandle(STD_OUTPUT_HANDLE); // 获取标准输出句柄
			if (hStdout == INVALID_HANDLE_VALUE) {
				throw Truntime_error(Tx("Failed to get standard output handle."));
			}
			hBuffer[0] = hStdout; // 默认缓冲区
			hBuffer[1] = CreateConsoleScreenBuffer( // 创建第二个缓冲区
				GENERIC_READ | GENERIC_WRITE,
				0,
				nullptr,
				CONSOLE_TEXTMODE_BUFFER,
				nullptr
			);
			if (hBuffer[1] == INVALID_HANDLE_VALUE) {
				throw Truntime_error(Tx("Failed to create secondary console buffer."));
			}

			// 设置缓冲区大小和窗口尺寸
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(hStdout, &csbi);
			bufferSize = csbi.dwSize;
			SMALL_RECT rect = csbi.srWindow;
			for (int i = 0; i < 2; i++) {
				SetConsoleScreenBufferSize(hBuffer[i], bufferSize);
				SetConsoleWindowInfo(hBuffer[i], TRUE, &rect);
			}

			// 初始化缓冲区索引
			currentBuffer = 0;
		}

		~ConsoleDoubleBuffer() {
			// 恢复默认缓冲区
			SetConsoleActiveScreenBuffer(hStdout);

			// 关闭创建的缓冲区
			CloseHandle(hBuffer[1]);
		}

	public:

		bool UpdataBuffer(Tstr& _Text) {
			try {
				// 绘制到当前缓冲区
				DWORD written;
				COORD coord = { 0, 0 };
				return WriteConsoleOutputCharacter(hBuffer[currentBuffer], _Text.c_str(), static_cast<DWORD>(_Text.size()), coord, &written);
			}
			catch (const std::exception& ex) {
#ifndef UNICODE
				lgc(Err, Format(Tx("Error: "), ex.what()));
#else
				lgc(Err, Format(Tx("Error: "), stow(ex.what())));
#endif
			}
		}

		void SwapBuffers(bool _bUpdataSuccessful) {
			if (_bUpdataSuccessful) {
				// 切换活动缓冲区
				currentBuffer = 1 - currentBuffer;
				SetConsoleActiveScreenBuffer(hBuffer[currentBuffer]);

				// 清空另一个缓冲区
				COORD coord = { 0, 0 };
				DWORD written;
				FillConsoleOutputCharacter(hBuffer[1 - currentBuffer], Tx(' '), bufferSize.X * bufferSize.Y, coord, &written);
				FillConsoleOutputAttribute(hBuffer[1 - currentBuffer], FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
					bufferSize.X * bufferSize.Y, coord, &written);
				SetConsoleCursorPosition(hBuffer[1 - currentBuffer], coord);
			}
		}

		HANDLE GetCurrentBufferHandle() const {
			return hBuffer[currentBuffer];
		}

	};
#endif
}


#endif
