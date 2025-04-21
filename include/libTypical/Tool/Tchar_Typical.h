#pragma once

#ifndef _TCHAR_H
#define _TCHAR_H

#include <libTypical/Tool/pch.h>


// Tchar----------------------------------------------------------------------------------------------------------
namespace Typical_Tool {
	namespace TcharStringManage {
		// 将 std::string 转换为 std::wstring
		template<class T = bool>
		std::wstring UTF8ToWstring(const std::string& _UTF8_String) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return converter.from_bytes(_UTF8_String);
		}

		// 将 std::wstring 转换为 std::string
		template<class T = bool>
		std::string WstringToUTF8(const std::wstring& _WString) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return converter.to_bytes(_WString);
		}
#define u8tow UTF8ToWstring
#define stow UTF8ToWstring
#define wtou8 WstringToUTF8
#define wtos WstringToUTF8
	}


#ifndef TEXT
#ifndef UNICODE
	// ""
#define TEXT(x) x

#else
#ifdef UTF8
	// ""
#define TEXT(x) x
#else
	// L""
#define TEXT(x) L ## x
#endif

#endif

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
#ifdef UTF8
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

#endif



#ifndef _DEBUG
#define _LOGERRORINFO(x) (x)
#else
#ifndef UNICODE
#define _LOGERRORINFO(x) (Tstr)TEXT("[") + TEXT("File: ") + __FILE__ + TEXT(" Line: ") + ToStr(__LINE__)+ TEXT("]") + x

#else
#ifdef UTF8
#define _LOGERRORINFO(x) (Tstr)TEXT("[") + TEXT("File: ") + __FILE__ + TEXT(" Line: ") + ToStr(__LINE__)+ TEXT("]") + x

#else
#define _LOGERRORINFO(x) (Tstr)TEXT("[") + TEXT("File: ") + stow(__FILE__) + TEXT(" Line: ") + ToStr(__LINE__)+ TEXT("]") + x
#endif
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


#define _Bracket(x) (Tstr)TEXT("[") + x + TEXT("]")
#define _Brace(x)	(Tstr)TEXT("{") + x + TEXT("}")


	class wruntime_error : public std::exception { // base of all runtime-error exceptions
	public:
		inline explicit wruntime_error(const std::wstring& _Message) : 
			std::exception(TcharStringManage::wtos(_Message).c_str()) {}

		inline explicit wruntime_error(const wchar_t* _Message) : 
			std::exception(TcharStringManage::wtos(_Message).c_str()) {}

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
#ifdef UTF8
	#define Truntime_error std::runtime_error

#else
	#define Truntime_error wruntime_error
#endif

#endif



	template<class ComputeValue, class Callable>
	void _IsValid_RunTime(ComputeValue _Value, Callable&& _ComputValueFunction, const Tstr& _ThrowInfo)
	{
		if (!_ComputValueFunction(_Value)) {
			throw Truntime_error(_LOGERRORINFO(_ThrowInfo));
		}
		return;
	}
}
namespace tytl = Typical_Tool;


#endif