#pragma once

#include <libTypical/Tool/pch.h>
#include <libTypical/Tool/Tchar_Typical.h>
#include <libTypical/Tool/StringManage.h>
#include <libTypical/Tool/Time_Typical.h>


#ifndef _LOG_H
#define _LOG_H


//#define _CONSOLE
#define _Thread
//#undef _WINDOWS


namespace Typical_Tool
{
	using namespace std;
	using namespace StringManage;



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
	*/
	class Log {
	private:
		inline static std::shared_ptr<Tofstream> LogFileStream_Out; //日志文件流 输出
		inline static mutex mutex_LogFileStream_Out;
		inline static queue<Tstr> LogFileWrite_Queue; //日志 WriteConfigFile队列
		inline static thread LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
		
		static atomic<bool> IsLogFileWriteThreadStop; //是否 停止日志文件写入线程

	private:
		static bool init; //初始化
		static bool IsLogFileWrite; //是否 写入日志文件
		static bool IsLogAllOutput; //是否 输出所有日志
		static bool ShowLog; //显示日志
		static bool ShowTime; //显示时间
		static bool SingleLogFile; //单一日志文件
		static bool Debug; //#define _Debug(或自定义的Debug) (Debug == true)

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

#ifndef _UE_
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
#endif
#endif
				
				//完成初始化
				init = true;
			}
		}


	public:
		static void LogWirte(const Tstr& _str)
		{
			if (IsLogFileWrite) { //写入文件
				lock_guard<mutex> QueueGuard(mutex_LogFileStream_Out);
				LogFileWrite_Queue.push(_str);
			}
		}


	private:
		void LogWirteToFile(std::shared_ptr<Tofstream> _LogFileStream_Out, const Tstr& _Log_FilePath)
		{
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

		static void StopLogWrite()
		{
			if (!IsLogFileWriteThreadStop.load()) {
				IsLogFileWriteThreadStop.store(true); //退出 条件
				LogFileProcessing.join(); //主线程等待 后台退出
				Terr << ANSIESC_YELLOW << Log_wr << Tx("Log: 日志写入完成! 程序退出...") << ANSIESC_RESET << Log_lf;
				//Time::wait_s<time::ms>(10); //主线程等待 后台退出
			}
		}

	private:

		static void Log_Out(const Tstr& _ANSIESC_Color, Tostream& _ostream, const Tstr& _str, const Tstr& _ANSIESC_RESET = ANSIESC_RESET, bool _IsWirteFile = false)
		{
			_ostream << _ANSIESC_Color; //在时间输出之前
			if (ShowTime) {
				_ostream << Time::GetFormatTime();
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
		static void Log_Out(const Tstr& _ANSIESC_Color, Tostream& _ostream, Tstr&& _str, const Tstr& _ANSIESC_RESET = ANSIESC_RESET, bool _IsWirteFile = false)
		{
			_ostream << _ANSIESC_Color; //在时间输出之前
			if (ShowTime) {
				_ostream << Time::GetFormatTime();
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
			case LogMessage::Tip: {
				if (this->CMD) {
					Log_Out(ANSIESC_GREEN, Tout, (Tstr)Log_ts + text + Log_lf);
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
					Log_Out(ANSIESC_YELLOW, Tout, (Tstr)Log_wr + text + Log_lf);
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
					Log_Out(ANSIESC_RED, Terr, (Tstr)Log_er + text + Log_lf, ANSIESC_RESET, true);
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
					Log_Out(Tx(""), Terr, text, Tx(""));

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
					Log_Out(Tx(""), Terr, text + Log_lf, Tx(""));

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
				Log_Out(Tx(""), Tout, (Tstr)Log_lf, Tx(""));
				
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
				if (this->Release) {
					Logs_ustr(text, lm());
				}
			}
		}
		/*void operator()(const Format& _FormatText, LogMessage (*lm)() = &lf)
		{
			if (ShowLog) {
#ifdef _DEBUG
				Logs_ustr(_FormatText.str(), lm());
				return;
#endif
				if (this->Release) {
					Logs_ustr(_FormatText.str(), lm());
				}
			}
		}*/
		void operator()(LogMessage (*lm)(), const Tstr& text)
		{
			if (ShowLog) {
#ifdef _DEBUG
				Logs_ustr(text, lm());
				return;
#endif
				if (this->Release) {
					Logs_ustr(text, lm());
				}
			}
		}
		/*void operator()(LogMessage (*lm)(), const Format& _FormatText)
		{
			if (ShowLog) {
#ifdef _DEBUG
				Logs_ustr(_FormatText.str(), lm());
				return;
#endif
				if (this->Release) {
					Logs_ustr(_FormatText.str(), lm());
				}
			}
		}*/
		void operator()()
		{
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
							Tstr Log_FileName = Time::GetFormatTime(Tx("%Y-%m-%d_%H-%M-%S_"), Tx(""), Tx(""));
							// ./Log/时间.txt  ||  ./时间.txt
							Log_FilePath = Log_FolderName + PATH_SLASH + Log_FileName + Log_FilePath;
						}
						else { // 不是目录
						}
					}
					else { //目录不存在
						std::filesystem::create_directory(Log_FolderName); //创建目录
						//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
						Tstr Log_FileName = Time::GetFormatTime(Tx("%Y-%m-%d_%H-%M-%S_"), Tx(""), Tx(""));
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

	/* 模式 mode : (_CONSOLE | _WINDOWS) && #ifdef _DEBUG
	* tip, war, Terr
	*/
	static Log lg(false, false);
	/* 模式 mode: (_CONSOLE | _WINDOWS) && #ifndef _DEBUG
	* tip, war, Terr
	*/
	static Log lgr(false, true);
	/* 模式 mode: (_CONSOLE) && #ifdef _DEBUG
	* tip, war, Terr
	*/
	static Log lgc(true, false);
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
}


#endif
