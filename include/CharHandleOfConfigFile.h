#pragma once

//配置文件的字符处理
#ifndef _CHARHANDLEOFCONFIGFILE_H
#define _CHARHANDLEOFCONFIGFILE_H

//#include "Typical_Tool.h"
#include "pch.h"
#include "CommonTools.h"
#include "Log.h"


namespace Typical_Tool {
	namespace StringHandling
	{
		enum  配置文件字符串格式
		{
			区域设置,
			区域内容,
			空行, //以换行符 或 空格开头
			错误 //字符串传入为空
		};
		using Ss = 配置文件字符串格式;

		// 配置文件的字符处理
		// 先运行 README() 看使用简介
		class  CharHandleOfConfigFile
		{
		private:
			Ustr 配置文件_路径;

			//区域总容器
			std::map<Ustr, std::map<Ustr, Ustr>> 区域总容器; //名称在前, 内容在后

			//区域设置_总数
			int 区域设置_总数;
			
			std::ofstream LogFileStream_Out;
			std::ifstream FileStream_In;

		public:
			// 自述: 打印使用简介的文本
			template<class T = bool>
			void README() const
			{
				lgc("编码问题:", lgm::ts);
				lgc("Windows: 使用之前先统一编码[Unicode(UTF-8 BOM)]: 源码编码, 执行编码, 解析编码", lgm::ts);
				lgc("其中, 执行与解析的编码已完成统一: UTF-8 BOM", lgm::ts);
				lgc("执行编码: char [] = \"这是中文\", 传入字符同理", lgm::ts);
				lgc("解析编码: system(\"chcp 65001\");", lgm::ts);
				lgc("源码编码的 MSVC 流程为: 另存文件为 UTF-8 BOM(带签名)", lgm::ts);
				lgc("文件->高级保存项(没有百度)-> Unicode(UTF-8 带签名): 不带 Windows可能会识别错误", lgm::ts);
				lgc("使用方式:", lgm::ts);
				lgc("1.创建 CharHandleOfConfigFile(传入文件路径) 对象.Init()", lgm::ts);
				lgc("2.使用 std::vector<Ustr>::push_back() 添加内容", lgm::ts);
				lgc("例. 文件显示: 屏幕高=1920  代码: Vector.push_back(\"屏幕高=1920\")", lgm::ts);
				lgc("3.使用 添加区域()", lgm::ts);
				lgc("例. 文件显示:[默认配置] 代码: 添加区域(\"默认配置\"), std::vector)", lgm::ts);
				lgc("4.使用 写入文件()", lgm::ts);
			}

			CharHandleOfConfigFile()
				: 区域设置_总数(0)
			{
			}

			~CharHandleOfConfigFile();

		public:
			// 初始化
			bool Init(const Ustr& 传入配置文件路径, bool 解析 = true);
			bool Init_Str(const Ustr& 传入配置文件路径, std::vector<Ustr>& 文件内容);

			// 获取 指定区域中所有的内容
			std::map<Ustr, Ustr> Get指定区域内容(const Ustr& 传入区域设置) const;
			// 获取 全部配置
			std::map<Ustr, std::map<Ustr, Ustr>> Get配置文件全内容() const;

			/* 写入区域设置项 vector
			* 区域设置传入格式(不用检查): 默认  ;"[]"
			* 区域内容传入格式(内容是带格式的字符串, 需要检查): str=char*  ;"="
			*/
			bool 添加区域(const Ustr& 传入区域设置, std::vector<Ustr>& 传入区域内容);

			// 删除区域设置中的一条内容: 删除至少一条内容时为 true, 否则为没有对应的内容可以删除
			bool 删除区域(const Ustr& 传入区域设置, const Ustr& 传入区域内容);
			// 删除一条区域设置: 删除至少一条内容时为 true, 否则为没有对应的内容可以删除; 需要传入 bool 以免误删
			bool 删除区域(const Ustr& 传入区域设置,  bool 确定删除);

			void 修改区域(const Ustr& 传入区域设置, const Ustr& 传入区域内容键, const Ustr& 传入区域内容值);

			bool 写入文件();
			bool 写入文件(std::vector<Ustr>& _文本内容);

			bool 创建文件();
			bool 创建文件(const Ustr& 新文件路径);

		private:
			bool 格式化(std::vector<Ustr>& 传入格式化容器);
			bool 解析(std::vector<Ustr>& 传入区域内容);
			配置文件字符串格式 格式识别(Ustr& 传入解析内容);

		private:
			// 区域设置 中括号字符串分离
			bool 区域设置解析(Ustr& 传入区域设置);
			// 区域内容 等号字符串分离
			bool 区域内容解析(Ustr& 传入区域内容, Ustr& 传出分离后区域内容_First, Ustr& 传出分离后区域内容_Second);

		public:

			/*r 以只读方式打开文件，该文件必须存在。
			r+ 以可读写方式打开文件，该文件必须存在。
			rb+ 读写打开一个二进制文件，只允许读写数据。
			w 打开只写文件，若文件存在则文件长度清为0，即该文件内容会消失。若文件不存在则建立该文件。
			w+ 打开可读写文件，若文件存在则文件长度清为0，即该文件内容会消失。若文件不存在则建立该文件。
			a 以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留。（EOF符保留）
			a+ 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留。 （原来的EOF符不保留）
			wb 只写打开或新建一个二进制文件；只允许写数据。
			wb+ 读写打开或建立一个二进制文件，允许读和写。
			ab+ 读写打开一个二进制文件，允许读或在文件末追加数据。
			rt+ 读写打开一个文本文件，允许读和写。
			wt+ 读写打开或着建立一个文本文件；允许读写。
			at+ 读写打开一个文本文件，允许读或在文本末追加数据。*/
			//static bool 文件操作(std::string 文件路径, std::string 文件打开方式 = "r", Ustr 日志 = "打开");
			//static bool 文件操作(FILE** 文件流, std::string 文件路径, std::string 文件打开方式 = "r", Ustr 日志 = "打开");
			
			/*
			* _FileMode:
			std::ios::out
			以输出模式打开文件. 文件内容会被清除(默认行为)
			std::ios::app
			以追加模式打开文件. 所有写入操作将追加到文件末尾, 不会清除文件的现有内容。
			std::ios::trunc
			以截断模式打开文件. 如果文件已存在, 文件内容会被清除. 这个模式通常与 std::ios::out 一起使用。
			std::ios::binary
			以二进制模式打开文件. 避免对文件内容进行文本转换(如换行符处理)
			*/
			bool 文件写入(std::ofstream& _FileStream_Out, const Ustr& _FilePath, std::vector<Ustr>& _内容, const Ustr& _文件编码 = "UTF-8BOM");
			bool 文件读取(std::ifstream& _FileStream_In, const Ustr& _FilePath, std::vector<Ustr>& _内容, const Ustr& _文件编码 = "UTF-8BOM");
		};
		using CfgFile = CharHandleOfConfigFile;
	}
}
namespace 工具库 = Typical_Tool;

#endif