#pragma once

//常用工具
#ifndef _STRINGMANAGE_H
#define _STRINGMANAGE_H


#include <libTypical/Tool/pch.h>



namespace Typical_Tool {
	//字符处理---------------------------------------------------------------------------------------------------------------
	namespace StringManage
	{
		inline bool IsRunTimeError = false;

		//字符转换-------------------------------------------------------------------------------------------------------
		inline std::string UTF8ToWstring_UTF8(const std::string& U8Str) { return U8Str; };
		std::wstring UTF8ToWstring(const std::string&);
		std::wstring UTF8ToWstring(std::string&&);
#ifdef u8tow
#undef u8tow
#endif
#ifdef stow
#undef stow
#endif

#ifdef UTF8
#define u8tow UTF8ToWstring_UTF8
#define stow UTF8ToWstring_UTF8
#else
#define u8tow UTF8ToWstring
#define stow UTF8ToWstring
#endif

		inline std::string WstringToUTF8_UTF8(const std::string& U8Str) { return U8Str; };
		std::string WstringToUTF8(const std::wstring&);
		std::string WstringToUTF8(std::wstring&&);
#ifdef wtou8
#undef wtou8
#endif
#ifdef wtos
#undef wtos
#endif

#ifdef UTF8
#define wtou8 WstringToUTF8_UTF8
#define wtos WstringToUTF8_UTF8
#else
#define wtou8 WstringToUTF8
#define wtos WstringToUTF8
#endif

		//编码转换---------------------------------------------------------------------------------------------------------
	}
}


#endif