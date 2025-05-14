#pragma once

//常用工具
#ifndef _STRINGMANAGE_H
#define _STRINGMANAGE_H


#include <TypicalTool/Tool/pch.h>



namespace Typical_Tool {
	//字符处理---------------------------------------------------------------------------------------------------------------
	namespace StringManage
	{
		inline bool IsRunTimeError = false;

		//字符转换-------------------------------------------------------------------------------------------------------
		template<class T = bool>
		std::string UTF8ToWstring_UTF8(const std::string& U8Str) { return U8Str; };

		TYPICALTOOL_API std::wstring UTF8ToWstring(const std::string&);
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

		TYPICALTOOL_API std::string WstringToUTF8(const std::wstring&);
#ifdef wtou8
#undef wtou8
#endif
#ifdef wtos
#undef wtos
#endif

#define wtou8 WstringToUTF8
#define wtos WstringToUTF8

		//编码转换---------------------------------------------------------------------------------------------------------
	}
}


#endif