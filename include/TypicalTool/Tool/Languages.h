#pragma once
#ifndef _LANGUAGES_H
#define _LANGUAGES_H


#include <TypicalTool/Tool/pch.h>
#include <TypicalTool/Tool/Log.h>
#include <TypicalTool/Tool/Json_Typical.h>


namespace Typical_Tool {
	class TYPICALTOOL_API Languages {
	private:
		Tstr RootLanguagesFile; //根语言文件
		Json::Value Root;
	};
}



#endif