#pragma once

//常用工具
#ifndef _STRINGMANAGE_H
#define _STRINGMANAGE_H


#include "pch.h"

namespace Typical_Tool {
	//字符处理---------------------------------------------------------------------------------------------------------------
	namespace StringManage
	{
		static bool IsRunTimeError = false;

		//字符转换-------------------------------------------------------------------------------------------------------
		std::wstring StringToWstring(const std::string& str);
		std::wstring StringToWstring(std::string&& str);
#define stow StringToWstring
		std::string WstringToString(const std::wstring& wstr);
		std::string WstringToString(std::wstring&& wstr);
#define wtos WstringToString

		//编码转换---------------------------------------------------------------------------------------------------------
	}
}



#endif