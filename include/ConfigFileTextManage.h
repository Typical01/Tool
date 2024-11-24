#pragma once

//配置文件的字符处理
#ifndef _CONFIGFILETEXTMANAGE_H
#define _CONFIGFILETEXTMANAGE_H

#include "Log.h"
#include "CommonTools.h"


namespace Typical_Tool {
	namespace StringManage
	{
		/* 配置字符串格式, 基础结构:
		*	[config]\n
		*	key=value\n
		*	key2=value2\n
		*/
		enum  ConfigStringFormat
		{
			Config, //配置
			ConfigItem, //配置项
			OtherChar, //控制字符: 不可显示字符
		};

		// 配置文件的字符处理
		class  ConfigFileTextManage
		{
		private:
			Tstr ConfigFilePath; //配置文件路径

			std::vector<Tstr> TextCache; //文本缓存

			std::map<Tstr, std::map<Tstr, Tstr>> ConfigMap; //总配置
			std::map<int, Tstr> OtherCharMap; //其他非配置项

			bool ShowManageLog = false; //显示配置处理过程日志
			bool FormatText = false;

		public:
			template<class T = bool>
			void README() const
			{
				lgc("编码问题:", lm::ts);
				lgc("Windows: 使用之前先统一编码[Unicode(UTF-8)]: 文件编码, 控制台/其它显示输出程序 编码, 源文件编码", lm::ts);
			}

			/* 输出: CMD
			* ShowManageLog: 显示配置处理过程日志
			*/
			ConfigFileTextManage(bool _ShowManageLog = true)
				: ShowManageLog(_ShowManageLog)
			{
			}

			~ConfigFileTextManage();

		public:
			/* 初始化
			* _Analyze: 解析文本为 配置格式
			*/
			bool Init(const Tstr& _ConfigFilePath, bool _Analyze = true);
			/* 初始化
			* _ReadText: 只读取文本
			*/
			bool InitText(const Tstr& _ConfigFilePath, std::vector<Tstr>& _ReadText);

			//设置 显示处理过程的日志
			void SetShowManageLog(bool _IsShowManageLog);

		public:
			//获取 配置项 Value
			Tstr GetConfigItem_Value(const Tstr& _Config, const Tstr& _ConfigItem);
			//获取 配置项 Key-Value
			std::map<Tstr, Tstr> GetConfigItem(const Tstr& _Config, const Tstr& _ConfigItem);
			//获取 配置
			std::map<Tstr, Tstr> GetConfig(const Tstr& _Config);
			//获取 全部配置
			std::map<Tstr, std::map<Tstr, Tstr>> GetConfigMap();
			//输出 配置文件所有内容
			void OutConfigFile_All() const;

			/* 添加 配置
			* 设置格式: "config"
			* 设置项格式: "key=value"
			*/
			bool AddConfig(const Tstr& _Config, std::vector<Tstr>& _ConfigItem);
			bool AddConfig(const Tstr& _Config, std::map<Tstr, Tstr>& _ConfigItem);

			//删除 配置项: 删除项时为 true, 否则为 false
			bool DeleteConfigItem(const Tstr& _Config, const Tstr& _ConfigItem);
			//删除 配置: 删除配置时为 true, 否则为 false; 需要二次确认
			bool DeleteConfig(const Tstr& _Config,  bool _IsDelete);

			//修改 配置项 Value
			bool SetConfigItemValue(const Tstr& _Config, const Tstr& _ConfigItem_Key, const Tstr& _ConfigItem_Value);
			//修改 配置项 Key
			bool SetConfigItemKey(const Tstr& _Config, const Tstr& _ConfigItem_Key, const Tstr& _ConfigItem_NewKey);
			//修改 配置
			bool SetConfig(const Tstr& _Config, const Tstr& _NewConfig);

			//创建文件
			bool CreateFile(const Tstr& _FileEncode = "UTF-8");
			bool CreateFile(const Tstr& _NewFilePath, const Tstr& _FileEncode = "UTF-8");

		public:
			/* 写入文件 : 格式化后的配置
			* _IsSureWrite: 强制写入
			*/
			bool WriteConfigFile(bool _IsSureWrite = true, const Tstr& _FileEncode = "UTF-8");
			//写入文件: 文本
			bool WriteTextFile(std::vector<Tstr>& _Text, const Tstr& _FileEncode = "UTF-8");
			//读取文件: 配置
			bool ReadConfigFile(const Tstr& _FileEncode = "UTF-8");
			//读取文件: 文本
			bool ReadConfigFile(std::vector<Tstr>& _Text, const Tstr& _FileEncode = "UTF-8");

		private:
			//将文本格式化为配置
			bool Format(std::vector<Tstr>& _FormatMap);
			/*
			* 解析文本内容
			* Analyze: 默认去掉换行符, 即str.size()-1
			* UTF-8 无法 str.find('\\n') -> return == str::npos
			*/
			bool Analyze();
			/* 解析格式
			* 除最后一行没有换行符以外
			* 其他每行删除 '\\n' 的字节数(1) 
			*/
			ConfigStringFormat AnalyzeFormat(Tstr& _AnalyzeConfig);

		private:
			//配置文本处理: 分离字符串中的中括号
			bool ConfigTextManage(Tstr& _Config);
			//配置项文本处理: 分离字符串中的等号; 默认去掉换行符, 即字符串.size() - 1
			bool ConfigItemTextManage(Tstr& _ConfigItem, Tstr& _AnalyzeLaterConfigItem_Key, Tstr& _AnalyzeLaterConfigItem_Value);

		public:
			//配置格式示例
			void ConfigFormatSample();
			void AddConfig_FormatSampleText();
		};
		using CfgFile = ConfigFileTextManage;


		//* Encode: 编码
		//写入文件
		bool WriteFile(const Tstr& _ConfigFilePath, std::vector<Tstr>& _WriteText, const Tstr& _FileEncode = "UTF-8");
		//* Encode: 编码
		//读取文件
		bool ReadFile(const Tstr& _ConfigFilePath, std::vector<Tstr>& _ReadText, const Tstr& _FileEncode = "UTF-8");
	}
}
namespace Tool_lib = Typical_Tool;

#endif

/*
* FileMode:
std::ios::out
以输出模式打开文件._FileText会被清除(默认行为)
std::ios::app
以追加模式打开文件.所有写入操作将追加到文件末尾, 不会清除文件的现有内容。
std::ios::trunc
以截断模式打开文件.如果文件已存在, _FileText会被清除.这个模式通常与 std::ios::out 一起使用。
std::ios::binary
以二进制模式打开文件.避免对_FileText进行文本转换(如换行符处理)
*/

/*r 以只读方式打开文件，该文件必须存在。
r+ 以可读写方式打开文件，该文件必须存在。
rb+ 读写打开一个二进制文件，只允许读写数据。
w 打开只写文件，若文件存在则文件长度清为0，即该_FileText会消失。若文件不存在则建立该文件。
w+ 打开可读写文件，若文件存在则文件长度清为0，即该_FileText会消失。若文件不存在则建立该文件。
a 以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留。（EOF符保留）
a+ 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留。 （原来的EOF符不保留）
wb 只写打开或新建一个二进制文件；只允许写数据。
wb+ 读写打开或建立一个二进制文件，允许读和写。
ab+ 读写打开一个二进制文件，允许读或在文件末追加数据。
rt+ 读写打开一个文本文件，允许读和写。
wt+ 读写打开或着建立一个文本文件；允许读写。
at+ 读写打开一个文本文件，允许读或在文本末追加数据.
*/