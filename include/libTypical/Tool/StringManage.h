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
		std::wstring UTF8ToWstring(const std::string&);
		std::wstring UTF8ToWstring(std::string&&);
#define u8tow UTF8ToWstring
#define stow UTF8ToWstring
		std::string WstringToUTF8(const std::wstring&);
		std::string WstringToUTF8(std::wstring&&);
#define wtou8 WstringToUTF8
#define wtos WstringToUTF8

		//编码转换---------------------------------------------------------------------------------------------------------
	}
}


#endif