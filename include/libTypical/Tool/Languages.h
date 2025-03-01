#pragma once
#ifndef _LANGUAGES_H
#define _LANGUAGES_H


#include <libTypical/Tool/pch.h>
#include <libTypical/Tool/Log.h>
#include <libTypical/Tool/Json_Typical.h>


namespace Typical_Tool {
	class Languages {
	private:
		Tstr RootLanguagesFile; //根语言文件
		Json::Value Root;
	};
}



#endif