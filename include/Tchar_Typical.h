#pragma once

#ifndef _TCHAR_H
#define _TCHAR_H

#include "pch.h"


// Tchar----------------------------------------------------------------------------------------------------------
namespace Typical_Tool {
	using namespace std;

#ifndef _T
#ifndef _WCHAR
	// ""
#define _T(x) x
#else
	// L""
#define _T(x) L ## x
#endif
#endif




//控制台字符颜色
#define _ANSIESC_CONSOLE_CHAR_COLOR
#ifdef _ANSIESC_CONSOLE_CHAR_COLOR
#ifndef _WCHAR
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
#ifndef _WCHAR
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


#ifndef _WCHAR
#define Tchar char
#define Tstr std::string
#define To_string std::to_string
#define Tstrlen strlen
#define Tostream std::ostream
#define Tofstream std::ofstream
#define Tostringstream std::ostringstream
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
#define To_string std::to_wstring
#define Tstrlen wcslen
#define Tostream std::wostream
#define Tofstream std::wofstream
#define Tostringstream std::wostringstream
#define Tifstream std::wifstream
#define Tfstream std::wfstream
#define Tistringstream std::wistringstream
#define Tostringstream std::wostringstream
#define Tfopen_s _wfopen_s
#define Tfputs std::fputws
#define Tfgets std::fgetws

#endif


#ifdef _WINDOWS
#ifndef _WCHAR
#define PATH_SLASH "\\"
#else
#define PATH_SLASH L"\\"
#endif

#else
#ifndef _WCHAR
#define PATH_SLASH "/"
#else
#define PATH_SLASH L"/"
#endif
#endif


#ifndef _WCHAR
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

#ifndef _WCHAR
#define _Bracket(x) (Tstr)"[" + x + "]"
#define _Brace(x) (Tstr)"{" + x + "}"
#else
#define _Bracket(x) (Tstr)L"[" + x + L"]"
#define _Brace(x) (Tstr)L"{" + x + L"}"
#endif


#ifndef _WCHAR
	inline Tostream& Tout = std::cout;
	inline Tostream& Terr = std::cerr;
#else
	inline Tostream& Tout = std::wcout;
	inline Tostream& Terr = std::wcerr;
#endif

}



#endif