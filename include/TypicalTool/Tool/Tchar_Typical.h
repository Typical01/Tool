#pragma once

#ifndef _TCHAR_H
#define _TCHAR_H

#include <TypicalTool/Tool/pch.h>
#include <TypicalTool/Tool/StringManage.h>

//#define UTF8

// Tchar----------------------------------------------------------------------------------------------------------
namespace Typical_Tool {

#ifndef UNICODE
	// ""
#define TEXT(x) x
#else

	// L""
#define TEXT(x) L ## x
#endif



#ifndef UNICODE

#define Tchar			char
#define Tstr			std::string
#define ToStr			std::to_string
#define PathToStr		std::filesystem::path::string
#define Tstrlen			strlen
#define Tostream		std::ostream
#define Tiostream		std::iostream
#define Tofstream		std::ofstream
#define Tifstream		std::ifstream
#define Tfstream		std::fstream
#define Tistringstream	std::istringstream
#define Tostringstream	std::ostringstream
#define Tfopen_s		fopen_s
#define Tfputs			std::fputs
#define Tfgets			std::fgets
#else

#define Tchar			wchar_t
#define Tstr			std::wstring
#define ToStr			std::to_wstring
#define PathToStr		std::filesystem::path::wstring
#define Tstrlen			wcslen
#define Tostream		std::wostream
#define Tiostream		std::wiostream
#define Tofstream		std::wofstream
#define Tifstream		std::wifstream
#define Tfstream		std::wfstream
#define Tistringstream	std::wistringstream
#define Tostringstream	std::wostringstream
#define Tfopen_s		_wfopen_s
#define Tfputs			std::fputws
#define Tfgets			std::fgetws
#endif



#ifndef _DEBUG

#define _LOGERRORINFO(x) (x)
#else

#ifndef UNICODE
#define _LOGERRORINFO(x) (Tstr)TEXT("[") + TEXT("File: ") + __FILE__ + TEXT(" Line: ") + ToStr(__LINE__)+ TEXT("]") + x
#else

#define _LOGERRORINFO(x) (Tstr)TEXT("[") + TEXT("File: ") + stow(__FILE__) + TEXT(" Line: ") + ToStr(__LINE__)+ TEXT("]") + x
#endif
#endif


//控制台字符颜色
#define _ANSIESC_CONSOLE_CHAR_COLOR

#ifdef _ANSIESC_CONSOLE_CHAR_COLOR
#define ANSIESC_RESET	TEXT("\033[0m")
#define ANSIESC_GREEN	TEXT("\033[32m")
#define ANSIESC_YELLOW	TEXT("\033[33m")
#define ANSIESC_RED		TEXT("\033[31m")
#else

#define ANSIESC_RESET	TEXT("")
#define ANSIESC_GREEN	TEXT("")
#define ANSIESC_YELLOW	TEXT("")
#define ANSIESC_RED		TEXT("")
#endif


#ifndef _WINDOWS
	// 定义虚拟类型
	using DWORD		= unsigned long;
	using WORD		= unsigned short;
	using HANDLE	= void*;
#endif


#ifdef _WINDOWS

#define PATH_SLASH TEXT("\\")
#else

#define PATH_SLASH TEXT("/")
#endif


#define Log_Tips		TEXT("[INFO]    ")
#define Log_Warning		TEXT("[WARNING] ")
#define Log_Error		TEXT("[ERROR]   ")
#define Log_Text		TEXT("[TEXT]    ")
#define Log_Tab			TEXT("          ")
#define Log_LineFeed	TEXT("\n")



	class TYPICALTOOL_API wruntime_error : public std::exception { // base of all runtime-error exceptions
	public:
		inline explicit wruntime_error(const std::wstring& _Message) : 
			std::exception(StringManage::wtos(_Message).c_str()) {}

		inline explicit wruntime_error(const wchar_t* _Message) : 
			std::exception(StringManage::wtos(_Message).c_str()) {}

#if !_HAS_EXCEPTIONS
	protected:
		void _Doraise() const override { // perform class-specific exception handling
			_RAISE(*this);
		}
#endif // !_HAS_EXCEPTIONS
	};
#ifndef UNICODE

	#define Truntime_error std::runtime_error
#else

	#define Truntime_error wruntime_error
#endif


	class TypicalException : public std::exception {
#ifndef UNICODE
	private:
		std::string Message;
	public:
		explicit TypicalException(const std::string& Message) : Message(Message) {}
		const char* Whats() const noexcept {
			return Message.c_str();
		}
#else
	private:
		Tstr Message;
	public:
		explicit TypicalException(const std::string& Message) : Message(StringManage::stow(Message)) {}
		const Tchar* Whats() const noexcept {
			return Message.c_str();
		}
#endif
	};


	template<class ComputeValue, class Callable>
	TYPICALTOOL_API void _IsValid_RunTime(ComputeValue _Value, Callable&& _ComputValueFunction, const Tstr& _ThrowInfo)
	{
		if (!_ComputValueFunction(_Value)) {
			throw Truntime_error(_LOGERRORINFO(_ThrowInfo));
		}
		return;
	}
}
namespace tytl = Typical_Tool;


#endif