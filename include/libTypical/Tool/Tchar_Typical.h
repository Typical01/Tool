#pragma once

#ifndef _TCHAR_H
#define _TCHAR_H

#include <libTypical/Tool/pch.h>


// Tchar----------------------------------------------------------------------------------------------------------
namespace Typical_Tool {
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


#define __WFILE__ L##__FILE__
#define __WLINE__ L##__LINE__

#ifndef _DEBUG
#define _LOGERRORINFO(x) (x)
#define _LOGERRORINFO_T(x) (x)
#else
#define _LOGERRORINFO(x) (std::string)"[" + __FILE__ + "->" + std::to_string(__LINE__ )+ "]" + x
#define _LOGERRORINFO_W(x) (std::wstring)L"[" + __WFILE__ + L"->" + std::to_wstring(__WLINE__ )+ L"]" + x
#endif

	


//控制台字符颜色
#define _ANSIESC_CONSOLE_CHAR_COLOR
#ifdef _ANSIESC_CONSOLE_CHAR_COLOR
#ifndef UNICODE
#define ANSIESC_RESET "\033[0m"
#define ANSIESC_GREEN "\033[32m"
#define ANSIESC_YELLOW "\033[33m"
#define ANSIESC_RED "\033[31m"
#else

#define ANSIESC_RESET L"\033[0m"
#define ANSIESC_GREEN L"\033[32m"
#define ANSIESC_YELLOW L"\033[33m"
#define ANSIESC_RED L"\033[31m"
#endif

#else
#ifndef UNICODE
#define ANSIESC_RESET ""
#define ANSIESC_GREEN ""
#define ANSIESC_YELLOW ""
#define ANSIESC_RED ""
#else

#define ANSIESC_RESET L""
#define ANSIESC_GREEN L""
#define ANSIESC_YELLOW L""
#define ANSIESC_RED L""
#endif
#endif


#ifndef _WINDOWS
	// 定义虚拟类型
	using DWORD = unsigned long;
	using WORD = unsigned short;
	using HANDLE = void*;
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


#ifdef _WINDOWS
#ifndef UNICODE
#define PATH_SLASH "\\"
#else
#define PATH_SLASH L"\\"
#endif

#else
#ifndef UNICODE
#define PATH_SLASH "/"
#else
#define PATH_SLASH L"/"
#endif
#endif


#ifndef UNICODE
#define Log_ts "[INFO]    "
#define Log_wr "[WARNING] "
#define Log_er "[ERROR]   "
#define Log_tx "[TEXT]    "
#define Log_lf "\n"
#else
#define Log_ts L"[INFO]    "
#define Log_wr L"[WARNING] "
#define Log_er L"[ERROR]   "
#define Log_tx L"[TEXT]    "
#define Log_lf L"\n"
#endif

#ifndef UNICODE
#define _Bracket(x) (Tstr)"[" + x + "]"
#define _Brace(x) (Tstr)"{" + x + "}"
#else
#define _Bracket(x) (Tstr)L"[" + x + L"]"
#define _Brace(x) (Tstr)L"{" + x + L"}"
#endif


#ifndef UNICODE
	inline Tostream& Tout = std::cout;
	inline Tostream& Terr = std::cerr;
#else
	inline Tostream& Tout = std::wcout;
	inline Tostream& Terr = std::wcerr;
#endif



	template<class ComputeValue, class Callable>
	void _IsValid_RunTime(ComputeValue _Value, Callable&& _ComputValueFunction, const std::string& _ThrowInfo)
	{
		if (!_ComputValueFunction(_Value)) {
			throw std::runtime_error(_LOGERRORINFO(_ThrowInfo));
		}
		else {
			return;
		}
	}
}
namespace tytool = Typical_Tool;


#endif