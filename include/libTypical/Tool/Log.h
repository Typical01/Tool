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
		Tstr Text;
		Tstr AnsiEsc_Reset;

	public:
		LogStringBuffer()
			: AnsiEsc_Color(Tx("")), TimeStr(Tx("")), Text(Tx("")), AnsiEsc_Reset(Tx("")) { }
		LogStringBuffer(const Tstr& AnsiEsc_Color, const Tstr& Text, const Tstr& AnsiEsc_Reset)
			: AnsiEsc_Color(AnsiEsc_Color), TimeStr(Tx("")), Text(Text), AnsiEsc_Reset(AnsiEsc_Reset) { }
		LogStringBuffer(const Tstr& AnsiEsc_Color, const Tstr& TimeStr, const Tstr& Text, const Tstr& AnsiEsc_Reset)
			: AnsiEsc_Color(AnsiEsc_Color), TimeStr(TimeStr), Text(Text), AnsiEsc_Reset(AnsiEsc_Reset) { }
	};
	template<class T = bool>
	Tostream& operator<<(Tostream& Ostream, const LogStringBuffer& LogStringBuffer) {
		return Ostream << LogStringBuffer.AnsiEsc_Color << LogStringBuffer.TimeStr 
			<< LogStringBuffer.Text << LogStringBuffer.AnsiEsc_Reset;
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
		Tip, //提示: Tips [INFO] | 绿色
		War, //警告: Warning [WARRING] | 黄色
		Err, //错误: Error [ERROR] | 红色
		End,  //结束: 不换行
		Lnf //换行: '\n'
	};
	typedef LogMessage lm;
	constexpr LogMessage ts() { return LogMessage::Tip; }
	constexpr LogMessage wr() { return LogMessage::War; }
	constexpr LogMessage er() { return LogMessage::Err; }
	constexpr LogMessage ed() { return LogMessage::End; }
	constexpr LogMessage lf() { return LogMessage::Lnf; }
	
	/* LogMessage = lm // 控制台颜色显示
	* 不需要颜色显示: #undef _ANSIESC_CONSOLE_CHAR_COLOR
	* Debug: 显示Log中的 Debug消息
	* _WIN32APP: 对于 Win32 app 显示指定控制台分离
	*/
	class Log {
	private:
		inline static std::shared_ptr<Tofstream> LogFileStream_Out; //日志文件流 输出
		inline static std::mutex mutex_LogFileStream_Out;
		inline static std::queue<LogStringBuffer> LogFileWrite_Queue; //日志 WriteConfigFile队列
		inline static std::thread LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
		static std::atomic<bool> IsLogFileWriteThreadStop; //是否 停止日志文件写入线程

		//保存格式
		inline static std::deque<std::pair<int, LogStringBuffer>> ConsoleMessages_Deque; // 控制台消息 队列
		inline static std::unordered_map<int, size_t> ConsoleMessages_Index; // ID 到索引的映射

	private:
		static bool init; //初始化
		static bool IsLogFileWrite; //是否 写入日志文件
		static bool IsLogAllOutput; //是否 输出所有日志
		static bool ShowLog; //显示日志
		static bool ShowTime; //显示时间
		static bool SingleLogFile; //单一日志文件
		static bool Debug; //#define _Debug(或自定义的Debug) (Debug == true)
		static bool bFlushConsoleOutput; //可刷新的控制台输出
		static long long PauseTime; //刷新间隔

		bool CMD; //控制台
		bool Release; //发行版

	public:
		Log(bool cmd, bool release)
			: CMD(cmd), Release(release)
		{
			Init();
		}

		~Log();

	public:
		/* 控制台初始化状态: false(需要初始化), true(跳过初始化)
		*/
		template<class Temp = bool>
		void Init() {
			//控制台初始化
			if (!init) {
				// 设置全局区域为 "zh_CN.UTF-8"
				locale::global(locale("zh_CN.UTF-8"));

				if (!Tout.good()) {
					throw Truntime_error(_LOGERRORINFO(Tx("Tout Not good!")));
				}
				if (!Terr.good()) {
					throw Truntime_error(_LOGERRORINFO(Tx("Terr Not good!")));
				}

				// 使用Config进行格式化
				Tout.imbue(locale()); // 使用全局Config
				Terr.imbue(locale());

				//Tout << "Log Config: zh_CN.UTF-8 完成!\n";

#ifdef _WIN32APP
#ifdef _WINDOWS
					//Windows 控制台编码修改: UTF-8

				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);

#ifndef _CONSOLE
				//分离控制台
				if (FreeConsole() == 0) {
					MessageBox(0, Tx("log: 分离控制台失败!"), Log_er, MB_ICONSTOP);
					MessageBox(0, Format(Tx("错误代码: %s"), ToStr(GetLastError())).str().c_str(), Log_er, MB_ICONSTOP);
				}
				else {
					if (!this->Release) {
						Terr << ANSIESC_GREEN << Log_ts << Tx("Log: 分离控制台.") << ANSIESC_RESET << Log_lf;
					}
				}


				//分配控制台: 当不是控制台程序时
				if (AllocConsole() == 0) {
					MessageBox(0, Tx("log: 分配控制台失败!"), Log_er, MB_ICONSTOP);
					MessageBox(0, Format(Tx("错误代码: %s"), ToStr(GetLastError())).str().c_str(), Log_er, MB_ICONSTOP);
				}
				else {
					if (!this->Release) {
						Terr << ANSIESC_GREEN << Log_ts << Tx("Log: 分配控制台.") << Log_lf;
					}
				}

				// 获取标准输出流的句柄
				FILE* FilePtr;
				// 重定向标准输出流
				freopen_s(&FilePtr, "CONOUT$", "w", stdout);
				if (FilePtr == nullptr) {
					MessageBox(0, Tx("日志: 重定向标准输出流失败!"), Log_er, MB_ICONSTOP);
					MessageBox(0, Format(Tx("错误代码: %s"), ToStr(GetLastError())).str().c_str(), Log_er, MB_ICONSTOP);
					return;
				}
				// 重定向标准错误流
				freopen_s(&FilePtr, "CONOUT$", "w", stderr);
				if (FilePtr == nullptr) {
					MessageBox(0, Tx("日志: 重定向标准错误流失败!"), Log_er, MB_ICONSTOP);
					MessageBox(0, Format(Tx("错误代码: %s"), GetLastError()).str().c_str(), Log_er, MB_ICONSTOP);
					return;
				}

				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);
#endif

				// 获取控制台窗口的句柄
				hConsole = GetConsoleWindow();
				EnableAnsiEscape();
#endif
#endif
				
				//完成初始化
				init = true;
			}
		}


	public:
		static void LogWirte(const LogStringBuffer& LogBuffer) {
			if (IsLogFileWrite) { //写入文件
				lock_guard<mutex> QueueGuard(mutex_LogFileStream_Out);
				LogFileWrite_Queue.push(LogBuffer);
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
		static void UpdataMessage(int ID, const LogStringBuffer& LogBuffer) {
			if (ID == -1) { // -1时, 不分配ID
				ConsoleMessages_Deque.push_back({ -1, LogBuffer });
			}

			if (ConsoleMessages_Index.find(ID) != ConsoleMessages_Index.end()) { //现有消息
				// 更新现有消息
				size_t index = ConsoleMessages_Index[ID];
				ConsoleMessages_Deque[index].second = LogBuffer;
			}
			else { // 新消息
				ConsoleMessages_Deque.push_back({ ID, LogBuffer }); //保存新消息的字符串
				ConsoleMessages_Index[ID] = ConsoleMessages_Deque.size() - 1; //将 新消息绑定到 索引
			}
		}

		static void UpdataCursorMessage(int Line, const Tstr& Message) {
			Tout << "\033[s";					// 保存光标位置 
			Tout << "\033[" << Line << ";1H";	// 移动光标到第 Line 行，第 1 列
			Tout << "\033[2K";					// 清除当前行内容
			Tout << Message;					// 输出新的消息
			Tout << "\033[u";					// 恢复光标位置
		}

	private:
		void LogWirteToFile(std::shared_ptr<Tofstream> _LogFileStream_Out, const Tstr& _Log_FilePath) {
			// 不退出, 且队列不为空
			while (1) {
				lock_guard<mutex> QueueGuard(mutex_LogFileStream_Out);

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
				Terr << ANSIESC_YELLOW << Log_wr << Tx("Log: 日志写入线程->退出!") << ANSIESC_RESET << Log_lf;
			}
		}

		static void StopLogWrite() {
			if (!IsLogFileWriteThreadStop.load()) {
				IsLogFileWriteThreadStop.store(true); //退出 条件
				LogFileProcessing.join(); //主线程等待 后台退出
				Terr << ANSIESC_YELLOW << Log_wr << Tx("Log: 日志写入完成! 程序退出...") << ANSIESC_RESET << Log_lf;
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
		static Tstr GetFormatTime(const Tstr& timeFormat = Tx("%Y-%m-%d %H:%M:%S"),
			const Tstr& textLeftFormat = Tx("["), const Tstr& textRigthFormat = Tx("]"))
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
			if (textLeftFormat == Tx("") && textRigthFormat == Tx("")) {
				return oss.str();
			}
			else {
				return textLeftFormat + oss.str() + textRigthFormat;
			}
		}

		static void wait(long long _Number, bool bShowLog) {
			Log::bFlushConsoleOutput = false;
			if (bShowLog) {
				Log_Out(ANSIESC_YELLOW, Terr, (Tstr)Log_wr + Format(Tx("等待: [%]ms"), _Number).str() + Log_lf, ANSIESC_RESET, -1, true);
			}
			Log_Out(ANSIESC_YELLOW, Terr, (Tstr)Log_wr + Format(Tx("等待: [%]ms"), _Number).str() + Log_lf, ANSIESC_RESET, -1, true);
			Log::bFlushConsoleOutput = true;
			std::chrono::milliseconds timeTarget = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()) + 
				std::chrono::milliseconds(_Number);
			while (timeTarget > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())) {}
		}

		static void Log_Out(const Tstr& _ANSIESC_Color, Tostream& _ostream, const Tstr& _str, const Tstr& _ANSIESC_RESET, const int& MessageKey = -1, bool _IsWirteFile = false) {
			LogStringBuffer tempLogBuffer;
			if (ShowTime) {
				tempLogBuffer = { _ANSIESC_Color, Log::GetFormatTime(), _str, _ANSIESC_RESET };
			}
			else {
				tempLogBuffer = { _ANSIESC_Color, _str, _ANSIESC_RESET };
			}

			if (!bFlushConsoleOutput) {
				_ostream << tempLogBuffer;

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
				UpdataMessage(MessageKey, tempLogBuffer);
			}
		}
		// _WINDOWS || _CONSOLE
		void Logs_ustr(const Tstr& text, const LogMessage& lm, const int& MessageKey) {
			switch (lm) {
			case LogMessage::Tip: {
				if (this->CMD) {
					Log_Out(ANSIESC_GREEN, Tout, (Tstr)Log_ts + text + Log_lf, ANSIESC_RESET, MessageKey);
				}
				else {
#ifdef _WINDOWS
#ifndef UNICODE
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_ts).c_str(), MB_OK);
#else
					MessageBoxW(NULL, text.c_str(), Log_ts, MB_OK);
#endif
#endif
				}

				break;
			}
			case LogMessage::War: {
				if (this->CMD) {
					Log_Out(ANSIESC_YELLOW, Tout, (Tstr)Log_wr + text + Log_lf, ANSIESC_RESET, MessageKey);
				}
				else {
#ifdef _WINDOWS
#ifndef UNICODE
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_wr).c_str(), MB_ICONWARNING);
#else
					MessageBoxW(NULL, text.c_str(), Log_wr, MB_ICONWARNING);
#endif
#endif
				}

				break;
			}
			case LogMessage::Err: {
				if (this->CMD) {
					Log_Out(ANSIESC_RED, Terr, (Tstr)Log_er + text + Log_lf, ANSIESC_RESET, MessageKey, true);
				}
				else {
#ifdef _WINDOWS
#ifndef UNICODE
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
#else
					MessageBoxW(NULL, text.c_str(), Log_er, MB_ICONSTOP);
#endif
#endif
				}

				break;
			}
			case LogMessage::End: {
				if (this->CMD) {
					Log_Out(Tx(""), Terr, text, Tx(""), MessageKey);

					return;
				}
				else {
#ifdef _WINDOWS
#ifndef UNICODE
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_tx).c_str(), MB_ICONSTOP);
#else
					MessageBoxW(NULL, text.c_str(), Log_tx, MB_ICONSTOP);
#endif
#endif
				}

				break;
			}
			case LogMessage::Lnf: {
				if (this->CMD) {
					Log_Out(Tx(""), Terr, text + Log_lf, Tx(""), MessageKey);

					return;
				}
				else {
#ifdef _WINDOWS
#ifndef UNICODE
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
			if (this->CMD) {
				Log_Out(Tx(""), Tout, (Tstr)Log_lf, Tx(""), -1);
				
				return;
			}
		}


	public:

		//显示/隐藏 Log消息
		void operator()(const Tstr& text, LogMessage (*lm)() = &lf, const int& MessageKey = -1)
		{
			if (ShowLog) {
#ifdef _DEBUG
				Logs_ustr(text, lm(), MessageKey);
				return;
#endif
				if (this->Release) {
					Logs_ustr(text, lm(), MessageKey);
				}
			}
		}
		void operator()(LogMessage (*lm)(), const Tstr& text, const int& MessageKey = -1)
		{
			if (ShowLog) {
#ifdef _DEBUG
				Logs_ustr(text, lm(), MessageKey);
				return;
#endif
				if (this->Release) {
					Logs_ustr(text, lm(), MessageKey);
				}
			}
		}
		/*
		* 输出 _LineNumber 个换行符
		*/
		void operator()(__int64 _LineNumber = 0)
		{
			for (auto i = 0; i < _LineNumber; i++) {
				if (ShowLog) {
#ifdef _DEBUG
					Logs_lgm();
					return;
#endif
					if (this->Release) {
						Logs_lgm();
					}
				}
			}
		}

	public:

		//设置日志显示
		static void SetShowLog(bool showLog)
		{
			if (Debug) {
				Terr << ANSIESC_YELLOW << Log_wr << Tx("Log 设置: 显示日志") << ANSIESC_RESET << Log_lf;
			}

			ShowLog = showLog;
		}
		//设置时间显示
		static void SetShowTime(bool showTime)
		{
			if (Debug) {
				Terr << ANSIESC_YELLOW << Log_wr << Tx("Log 设置: 显示时间") << ANSIESC_RESET << Log_lf;
			}

			ShowTime = showTime;
		}
		//设置是否为单一日志文件
		static void SetSingleLogFile(bool _SingleLogFile)
		{
			if (Debug) {
				Terr << ANSIESC_YELLOW << Log_wr << Tx("Log 设置: 单一日志文件: ") << std::boolalpha << _SingleLogFile << std::noboolalpha << ANSIESC_RESET << Log_lf;
			}

			SingleLogFile = _SingleLogFile;
		}
		//设置控制台显示: _WINDOWS(Win32 API)
		static void SetShowConsole(bool showConsole)
		{
#ifdef _WINDOWS
			if (Debug) {
				Terr << ANSIESC_YELLOW << Log_wr << Tx("Log 设置 : 显示控制台") << ANSIESC_RESET << Log_lf;
			}
			//显示/隐藏 窗口
			if (showConsole) {
				ShowWindow(hConsole, SW_SHOWDEFAULT);
			}
			else {
				ShowWindow(hConsole, SW_HIDE);
			}
#else
			if (Debug) {
				Terr << ANSIESC_YELLOW << Log_wr << Tx("[Warring]Log 设置 : 显示控制台->无效(#ifndef _WINDOWS)") << ANSIESC_RESET << Log_lf;
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
		* er: Error log level output
		* !=3: All(lm: ts/wr/tx) log level output
		*/
		void SetAllLogFileWrite(bool logFileWrite, const Tstr& _LogFileName, int logLevel = static_cast<int>(lm::Err))
		{
			IsLogFileWrite = logFileWrite;
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
							Log_FilePath = Log_FolderName + PATH_SLASH + Log_FileName + Log_FilePath;
						}
						else { // 不是目录
						}
					}
					else { //目录不存在
						std::filesystem::create_directory(Log_FolderName); //创建目录
						//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
						Tstr Log_FileName = Log::GetFormatTime(Tx("%Y-%m-%d_%H-%M-%S_"), Tx(""), Tx(""));
						// ./Log/时间.txt  ||  ./时间.txt
						Log_FilePath = Log_FolderName + PATH_SLASH + Log_FileName + Log_FilePath;
					}
				}

				//打开文件
				if (Debug) {
					Terr << ANSIESC_GREEN << Log_ts << Tx("Log: 日志输出文件名[") << Log_FilePath << Tx("]") << ANSIESC_RESET << Log_lf;
				}
				LogFileStream_Out = std::make_shared<Tofstream>(Log_FilePath, ios::out);
				if (!LogFileStream_Out->good()) {
					if (Debug) {
						Terr << ANSIESC_RED << Log_er << Tx("Log: [") << Log_FilePath << Tx("]打开文件失败!") << ANSIESC_RESET << Log_lf;
					}
					return;
				}
				else {
					if (Debug) {
						Tout << ANSIESC_GREEN << Log_ts << Tx("Log: [") << Log_FilePath << Tx("]打开文件成功!") << ANSIESC_RESET << Log_lf;
					}

					//Terr << "Log: 文件 开始写入!\n";
					//初始化: 日志WriteConfigFile线程
					std::thread LogFileWriteThread(&Log::LogWirteToFile, this, std::move(LogFileStream_Out), Log_FilePath);
					LogFileProcessing = std::move(LogFileWriteThread);
					if (Debug) {
						Terr << ANSIESC_GREEN << Log_ts << Tx("Log: 日志写入线程 启动!") << ANSIESC_RESET << Log_lf;
					}
				}
			}

			//输出所有级别
			if (logLevel != static_cast<int>(lm::Err)) {
				IsLogAllOutput = true;

				if (Debug) {
					Terr << ANSIESC_YELLOW << Log_wr
						<< Tx("Log: File Output level >> All <<")
						<< ANSIESC_RESET << Log_lf;
				}
			}
			else {
				IsLogAllOutput = false;

				if (Debug) {
					Terr << ANSIESC_YELLOW << Log_wr
						<< Tx("Log: File Output level >> Error <<")
						<< ANSIESC_RESET << Log_lf;
				}
			}
		}
	};


	/*class LogPackge {
	private:
		std::shared_ptr<Log> Log;

	public:
		LogPackge(std::shared_ptr<tytool::Log>& Log)
			: Log(Log) {}
		void operator()(const Tstr& text, LogMessage(*lm)() = &lf, const int& MessageKey = -1)
		{
			Log->operator()(text, lm, MessageKey);
		}
		void operator()(LogMessage(*lm)(), const Tstr& text, const int& MessageKey = -1)
		{
			Log->operator()(lm, text, MessageKey);
		}
		void operator()()
		{
			Log->operator()();
		}
	};*/

	/* 模式 mode : (_CONSOLE | _WINDOWS) && #ifdef _DEBUG
	* tip, war, Terr
	*/
	static Log lg(false, false);
	/* 模式 mode: (_CONSOLE | _WINDOWS) && #ifndef _DEBUG
	* tip, war, Terr
	*/
	static Log lgc(false, true);
	/* 模式 mode: (_CONSOLE) && #ifdef _DEBUG
	* tip, war, Terr
	*/
	static Log lgr(true, false);
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
			<< Tx("lgr(Format(Tx(\"%s :没有文件!\"), Path), wr); \n")
			<< Tx("_DEBUG: _WINDOWS / _CONSOLE\n")
			<< Tx("lg(\"关闭窗口\", ts); \n\n")
			<< Tx("#ifndef _DEBUG: _CONSOLE\n")
			<< Tx("lgcr(\"Code: 404\", er); \n")
			<< Tx("_DEBUG: _CONSOLE\n")
			<< Tx("lgc(\"Log.h\", lf); \n");
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

		bool UpdataBuffer(Tstr& Text) {
			try {
				// 绘制到当前缓冲区
				DWORD written;
				COORD coord = { 0, 0 };
				return WriteConsoleOutputCharacter(hBuffer[currentBuffer], Text.c_str(), static_cast<DWORD>(Text.size()), coord, &written);
			}
			catch (const std::exception& ex) {
#ifndef UNICODE
				lgc(er, Format(Tx("Error: "), ex.what()));
#else
				lgc(er, Format(Tx("Error: "), stow(ex.what())));
#endif
			}
		}

		void SwapBuffers(bool bUpdataSuccessful) {
			if (bUpdataSuccessful) {
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
