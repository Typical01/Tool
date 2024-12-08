#pragma once

//���ù���
#ifndef _STRINGMANAGE_H
#define _STRINGMANAGE_H


#include "pch.h"
#include "Log.h"


//�ַ�����---------------------------------------------------------------------------------------------------------------
namespace StringManage
{

	//�ַ�ת��-------------------------------------------------------------------------------------------------------
	std::wstring StringToWstring(const std::string& str);
	std::wstring StringToWstring(std::string&& str);
#define stow StringToWstring
	std::string WstringToString(const std::wstring& wstr);
	std::string WstringToString(std::wstring&& wstr);
#define wtos WstringToString

	//����ת��---------------------------------------------------------------------------------------------------------
}




#endif