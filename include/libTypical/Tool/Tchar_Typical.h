#pragma once

#ifndef _TCHAR_H
#define _TCHAR_H

#include <libTypical/Tool/pch.h>


// Tchar----------------------------------------------------------------------------------------------------------
namespace Typical_Tool {
	namespace TcharStringManage {
		// 将 std::string 转换为 std::wstring
		std::wstring UTF8ToWstring(const std::string& _UTF8_String);

		// 将 std::wstring 转换为 std::string
		std::string WstringToUTF8(const std::wstring& _WString);
#define u8tow UTF8ToWstring
#define stow UTF8ToWstring
#define wtou8 WstringToUTF8
#define wtos WstringToUTF8
	}

	using namespace std;



#ifndef Tx
#ifndef UNICODE
	// ""
#define Tx(x) x
#else
	// L""
#define Tx(x) L ## x
#endif
#endif


#ifndef UNICODE
#define Tchar char
#define Tstr std::string
#define ToStr std::to_string
#define Tstrlen strlen
#define Tostream std::ostream
#define Tofstream std::ofstream
#define Tifstream std::ifstream
#define Tfstream std::fstream
#define Tistringstream std::istringstream
#define Tostringstream std::ostringstream
#define Tfopen_s fopen_s
#define Tfputs std::fputs
#define Tfgets std::fgets

#else
#define Tchar wchar_t
#define Tstr std::wstring
#define ToStr std::to_wstring
#define Tstrlen wcslen
#define Tostream std::wostream
#define Tofstream std::wofstream
#define Tifstream std::wifstream
#define Tfstream std::wfstream
#define Tistringstream std::wistringstream
#define Tostringstream std::wostringstream
#define Tfopen_s _wfopen_s
#define Tfputs std::fputws
#define Tfgets std::fgetws

#endif


#ifndef _DEBUG
#define _LOGERRORINFO(x) (x)
#else
#ifndef UNICODE
#define _LOGERRORINFO(x) (Tstr)Tx("[") + __FILE__ + Tx("->") + ToStr(__LINE__)+ Tx("]") + x
#else
#define _LOGERRORINFO(x) (Tstr)Tx("[") + stow(__FILE__) + Tx("->") + ToStr(__LINE__)+ Tx("]") + x
#endif
#endif


//控制台字符颜色
#define _ANSIESC_CONSOLE_CHAR_COLOR
#ifdef _ANSIESC_CONSOLE_CHAR_COLOR
#define ANSIESC_RESET Tx("\033[0m")
#define ANSIESC_GREEN Tx("\033[32m")
#define ANSIESC_YELLOW Tx("\033[33m")
#define ANSIESC_RED Tx("\033[31m")

#else
#define ANSIESC_RESET Tx("")
#define ANSIESC_GREEN Tx("")
#define ANSIESC_YELLOW Tx("")
#define ANSIESC_RED Tx("")
#endif


#ifndef _WINDOWS
	// 定义虚拟类型
	using DWORD = unsigned long;
	using WORD = unsigned short;
	using HANDLE = void*;
#endif


#ifdef _WINDOWS
#define PATH_SLASH Tx("\\")

#else
#define PATH_SLASH Tx("/")
#endif


#define Log_Tips	Tx("[INFO]    ")
#define Log_Warning Tx("[WARNING] ")
#define Log_Error	Tx("[ERROR]   ")
#define Log_Text	Tx("[TEXT]    ")
#define Log_Tab		Tx("          ")
#define Log_LineFeed Tx("\n")


#define _Bracket(x) (Tstr)Tx("[") + x + Tx("]")
#define _Brace(x) (Tstr)Tx("{") + x + Tx("}")


#ifndef UNICODE
	inline Tostream& Tout = std::cout;
	inline Tostream& Terr = std::cerr;
#else
	inline Tostream& Tout = std::wcout;
	inline Tostream& Terr = std::wcerr;
#endif


	class wruntime_error : public exception { // base of all runtime-error exceptions
	public:
		explicit wruntime_error(const std::wstring& _Message) : exception(TcharStringManage::wtos(_Message).c_str()) {}

		explicit wruntime_error(const wchar_t* _Message) : exception(TcharStringManage::wtos(_Message).c_str()) {}

#if !_HAS_EXCEPTIONS
	protected:
		void _Doraise() const override { // perform class-specific exception handling
			_RAISE(*this);
		}
#endif // !_HAS_EXCEPTIONS
	};
#ifndef UNICODE
	#define Truntime_error runtime_error
#else
	#define Truntime_error wruntime_error
#endif



	template<class ComputeValue, class Callable>
	void _IsValid_RunTime(ComputeValue _Value, Callable&& _ComputValueFunction, const Tstr& _ThrowInfo)
	{
		if (!_ComputValueFunction(_Value)) {
			throw Truntime_error(_LOGERRORINFO(_ThrowInfo));
		}
		else {
			return;
		}
	}
}
namespace tytool = Typical_Tool;
namespace tytl = Typical_Tool;


#endif