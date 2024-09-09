#include "pch.h"

#include "CharHandleOfConfigFile.h"


using namespace Typical_Tool::StringHandling;

Typical_Tool::StringHandling::CharHandleOfConfigFile::~CharHandleOfConfigFile()
{
	this->LogFileStream_Out.close();
	this->FileStream_In.close();
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::Init(const Ustr& _配置文件路径, bool 解析)
{
	//保存路径
	this->配置文件_路径 = _配置文件路径;

	if (!解析)
	{
		return true;
	}

	std::vector<Ustr> 区域内容;

	//读取文件流
	std::ifstream 读取文件流(this->配置文件_路径, std::ios::binary);
	if (!文件读取(读取文件流, this->配置文件_路径, 区域内容)) {
		return false;
	}

	//读取配置文件内容
	if (this->解析(区域内容))
	{
		return true;
	}
	else
	{
		lgcr("文件中内容过少, 或不符合格式\n", lgm::wr);
		return false;
	}

	return true;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::Init_Str(const Ustr& _配置文件路径, std::vector<Ustr>& _文件内容)
{
	//保存路径
	this->配置文件_路径 = _配置文件路径;

	//读取文件流
	std::ifstream 读取文件流(this->配置文件_路径, std::ios::binary);
	if (!文件读取(读取文件流, this->配置文件_路径, _文件内容)) {
		return false;
	}

	return true;
}

std::map<Ustr, Ustr> Typical_Tool::StringHandling::CharHandleOfConfigFile::Get指定区域内容(const Ustr& 传入区域设置) const
{
	auto temp = this->区域总容器.find(传入区域设置);
	if (temp != this->区域总容器.end()) {
		return temp->second;
	}
	return std::map<Ustr, Ustr>();
}

std::map<Ustr, std::map<Ustr, Ustr>> Typical_Tool::StringHandling::CharHandleOfConfigFile::Get配置文件全内容() const
{
	return this->区域总容器;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::添加区域(const Ustr& 传入区域设置, std::vector<Ustr>& 传入区域内容)
{
	//重复的内容需要先删除, 再添加
	std::map<Ustr, Ustr> 重复区域设置;
	auto 重复区域 = this->区域总容器.find(传入区域设置);
	if (重复区域 != this->区域总容器.end()) {
		重复区域设置 = 重复区域->second;
		删除区域(传入区域设置, true);
	}

	std::vector<Ustr> temp解析后的区域内容_First(传入区域内容.size() + 重复区域设置.size(), "");
	std::vector<Ustr> temp解析后的区域内容_Second(传入区域内容.size() + 重复区域设置.size(), "");
	std::map<Ustr, Ustr> 解析后的区域与内容;
	int temp计数 = 0;

	//std::map<Ustr, Ustr> temp解析后的区域内容;
	for (auto tempStr = 传入区域内容.begin(); tempStr != 传入区域内容.end(); tempStr++)
	{
		if (this->区域内容解析(*tempStr, temp解析后的区域内容_First[temp计数], temp解析后的区域内容_Second[temp计数]))
		{
			temp计数++;
		}
	}

	//临时存放
	std::vector<Ustr> tempStrVec;
	for (auto tempStr = 重复区域设置.begin(); tempStr != 重复区域设置.end(); tempStr++)
	{
		tempStrVec.push_back(tempStr->first + tempStr->second);
	}

	//再次解析, 并将其添加到区域内容中
	for (auto tempStr = tempStrVec.begin(); tempStr != tempStrVec.end(); tempStr++)
	{
		if (this->区域内容解析(*tempStr, temp解析后的区域内容_First[temp计数], temp解析后的区域内容_Second[temp计数]))
		{
			temp计数++;
		}
	}

	lgc("区域设置: " + (Ustr)传入区域设置, lgm::wr);

	for (int i = 0; i < temp计数; i++)
	{
		//保存解析后的区域与内容
		解析后的区域与内容.insert(std::make_pair(temp解析后的区域内容_First[i], temp解析后的区域内容_Second[i]));

		lgc("first: [" + temp解析后的区域内容_First[i] + "] |second:[" + temp解析后的区域内容_Second[i] + "]");
	}

	std::pair<std::map<Ustr, std::map<Ustr, Ustr>>::iterator, bool> tempPair \
		= this->区域总容器.emplace(std::make_pair(传入区域设置, 解析后的区域与内容));

	if (!tempPair.second)
	{
		lg(("添加区域: " + (Ustr)传入区域设置 + "失败!").c_str(), lgm::er);
		return false;
	}

	return true;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::删除区域(const Ustr& 传入区域设置, const Ustr& 传入区域内容)
{
	auto temp = this->区域总容器.find(传入区域设置);
	if (temp != this->区域总容器.end()) {
		temp->second.erase(传入区域内容);
		return true;
	}
	return false;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::删除区域(const Ustr& 传入区域设置, bool 确定删除)
{
	if (this->区域总容器.erase(传入区域设置))
	{
		return true;
	}
	return false;
}

void Typical_Tool::StringHandling::CharHandleOfConfigFile::修改区域(const Ustr& 传入区域设置, const Ustr& 传入区域内容键, const Ustr& 传入区域内容值)
{
	auto temp = this->区域总容器.find(传入区域设置);
	if (temp != this->区域总容器.end()) {
		temp->second.at(传入区域内容键) = 传入区域内容值;
	}
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::区域内容解析(Ustr& 传入区域内容, Ustr& 传出分离后区域内容_First, Ustr& 传出分离后区域内容_Second)
{
	//传入 传入区域内容 不为空时
	if (!传入区域内容.empty())
	{
		//格式是否非法
		int temp非法格式_等于号 = 传入区域内容.find("=");
		if (temp非法格式_等于号 == Ustr::npos)
		{
			lgc(("区域内容: 格式错误" + 传入区域内容).c_str(), lgm::wr);
			return false;
		}

		int temp等于号位置 = temp非法格式_等于号 + 1; //找到 "=" 号的元素位置, string.find 返回下标位置
		int temp值长度 = 传入区域内容.length() - temp等于号位置; //去掉 "="号后剩余的元素个数
		Ustr temp键(temp等于号位置 - 1, ' '); //初始化
		Ustr temp值(temp值长度, ' '); //初始化

		//保存 键
		for (int i = 0; i < temp等于号位置; i++)
		{
			temp键[i] = 传入区域内容[i];
		}
		//保存 值: 内容从"="之后(+1)开始, string.find 返回下标位置
		for (int i = temp等于号位置; i < 传入区域内容.length(); ++i) //先加 加到'='之后
		{
			//从0开始保存
			temp值[i - temp等于号位置] = 传入区域内容[i];
		}
		//找到 temp值后的 '\n', 并删除: 避免使用异常
		if (temp值.find("\n") != Ustr::npos)
		{
			temp值.erase(temp值.size() - 1);
			//lgc(("temp值: [" + temp值 + "]").c_str());
		}

		传出分离后区域内容_First = temp键;
		传出分离后区域内容_Second = temp值;

		return true;
	}

	lgc("区域内容解析: 传入为空!", lgm::wr);

	return false;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::文件读取(
	std::ifstream& _FileStream_In, 
	const Ustr& _FilePath, 
	std::vector<Ustr>& _内容, 
	const Ustr& _文件编码)
{
	try {
		this->FileStream_In = std::move(_FileStream_In);

		if (!FileStream_In) {
			lgcr((Ustr)_FilePath + " 打开文件失败!\n", lgm::er);
			return false;
		}
		lgcr((Ustr)_FilePath + " 打开文件成功!\n", lgm::ts);
		lgcr("文件: 开始读取!\n", lgm::ts);

		// 读取 BOM（如果存在）
		if (_文件编码 == "UTF-8BOM") {
			std::vector<unsigned char> bom(3);
			FileStream_In.read(reinterpret_cast<char*>(bom.data()), 3);

			// 检查 BOM 是否为 UTF-8 BOM (EF BB BF)
			if (bom.size() == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
				lgcr("检测到UTF-8 BOM\n");
			}
			else {
				// 如果文件没有 BOM，返回文件流的读位置
				FileStream_In.clear(); // 清除 EOF 标志
				FileStream_In.seekg(0);
			}
		}

		std::string line;
		while (std::getline(FileStream_In, line)) {
			_内容.push_back(line);
		}

		return true;
	}
	catch (...) {
		lgcr("bool Typical_Tool::StringHandling::CharHandleOfConfigFile::文件读取()", lgm::er);
	}
}
bool Typical_Tool::StringHandling::CharHandleOfConfigFile::文件写入(
	std::ofstream& _FileStream_Out, 
	const Ustr& _FilePath, 
	std::vector<Ustr>& _内容, 
	const Ustr& _文件编码)
{
	try {
		this->LogFileStream_Out = std::move(_FileStream_Out);

		if (!LogFileStream_Out) {
			lgcr((Ustr)_FilePath + " 打开文件失败!\n", lgm::er);
			return false;
		}
		lgcr((Ustr)_FilePath + " 打开文件成功!\n", lgm::ts);
		lgcr("文件: 开始写入!\n", lgm::ts);

		if (_文件编码 == "UTF-8BOM") {
			// 写入 UTF-8 BOM (EF BB BF)
			lgcr("写入 UTF-8 BOM编码(EF BB BF)\n");

			LogFileStream_Out.put(0xEF);
			LogFileStream_Out.put(0xBB);
			LogFileStream_Out.put(0xBF);
		}

		for (const Ustr& tempStr : _内容) {
			LogFileStream_Out << tempStr;
		}

		return true;
	}
	catch (...) {
		lgcr("bool Typical_Tool::StringHandling::CharHandleOfConfigFile::文件写入()", lgm::er);
	}
}
bool Typical_Tool::StringHandling::CharHandleOfConfigFile::区域设置解析(Ustr& 传入区域设置)
{
	//vector 不为空时
	if (!传入区域设置.empty())
	{
		//格式是否非法
		int temp非法格式_左括号 = 传入区域设置.find("[");
		int temp非法格式_右括号 = 传入区域设置.find("]");
		if (temp非法格式_左括号 == Ustr::npos || temp非法格式_右括号 == Ustr::npos)
		{
			lgc("区域设置: 格式错误", lgm::wr);
			return false;
		}

		int 左括号的元素位置 = temp非法格式_左括号; //左括号的元素位置
		int 右括号的元素位置 = temp非法格式_右括号; //右括号的元素位置
		Ustr 区域设置名称(temp非法格式_右括号 - 1, ' '); //区域设置名称

		//解析 [传入区域设置]
		//保存ConfigNmae
		for (int temp_Int = 0;
			temp_Int < 右括号的元素位置; ++temp_Int) //find返回 0, 但是 0为 '['
		{
			//保存 区域设置名称
			区域设置名称[temp_Int] = 传入区域设置[temp_Int + 1];
		}
		//lgc(("区域设置名称: [" + 区域设置名称 + "]").c_str());
		传入区域设置 = 区域设置名称;

		return true;
	}

	lgc("区域设置解析: 传入为空!", lgm::wr);
	return false;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::解析(std::vector<Ustr>& 传入区域内容)
{
	//vec非空
	if (!传入区域内容.size() < 2)
	{
		Ustr temp解析后内容_First;
		Ustr temp解析后内容_Second;
		std::map<Ustr, Ustr> temp解析后区域与内容;
		Ustr 区域设置;

		/* 区域划分 */
		for (int i = 0; i < 传入区域内容.size(); i++)
		{
			Ustr temp解析内容 = 传入区域内容[i];
			Ss tempSs = 格式识别(temp解析内容);

			//非空行 无错误
			if (tempSs != Ss::空行 && tempSs != Ss::错误) //非空格字符和换行符
			{
				//区域设置
				if (tempSs == Ss::区域设置)
				{
					//区域设置计数增加
					this->区域设置_总数++;

					//是否到下一个区域设置
					if (this->区域设置_总数 > 1)
					{
						//保存到 总容器
						this->区域总容器.insert(std::make_pair(区域设置, temp解析后区域与内容));

						//保存后刷新
						temp解析后区域与内容.clear();
					}

					//做 区域设置的解析
					if (this->区域设置解析(传入区域内容[i]))
					{
						区域设置 = 传入区域内容[i];
					}
					else //空行 记录并跳过
					{
						//记录 首字符为空白的行
						//this->空行位置统计.insert(i);
					}

					//直接跳到下次
					continue;
				}
				else //区域设置 字符行
				{
					if (this->区域设置_总数 > 0)
					{
						//找到对应区域设置位置, 内容格式非法则 不保存
						if (this->区域内容解析(temp解析内容, temp解析后内容_First, temp解析后内容_Second))
						{
							temp解析后区域与内容.insert(std::make_pair(temp解析后内容_First, temp解析后内容_Second));
						}
						else //非法格式行
						{
							//记录 首字符为空白的行
							//this->空行位置统计.insert(i);
							//lg("空行: " + i, lgm::wr);
						}

						//直接跳到下次
						continue;
					}
				}
			}
			else if (tempSs == Ss::空行)
			{
				//记录 首字符为空白的行
				//this->空行位置统计.insert(i);
			}
			else if (tempSs == Ss::错误)
			{

			}
		}
		//保存到 总容器
		this->区域总容器.insert(std::make_pair(区域设置, temp解析后区域与内容));

		return true;
	}
	else
	{
		lgc("解析: 传入为空!", lgm::wr);
	}
	return false;
}

配置文件字符串格式 Typical_Tool::StringHandling::CharHandleOfConfigFile::格式识别(Ustr& 传入解析内容)
{
	if (!传入解析内容.empty())
	{
		//首字符符号
		Uchar temp区域符号 = 传入解析内容[0];

		//字符串是否非法
		if (temp区域符号 != _T(' ') && temp区域符号 != _T('\n')) //非空格符和换行符
		{
			//首字符 为区域设置名称划分符号 '['
			if (temp区域符号 == _T('['))
			{
				return Ss::区域设置;
			}
			return Ss::区域内容;
		}
		else //空字符串
		{
			return Ss::空行;
		}
	}
	else
	{
		lgc("格式识别: 传入为空!", lgm::wr);
		return Ss::错误;
	}
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::格式化(std::vector<Ustr>& 传入格式化容器)
{
	if (this->区域总容器.size() > 0)
	{
		for (auto 区域设置 = this->区域总容器.begin(); 区域设置 != this->区域总容器.end(); 区域设置++)
		{
			if (区域设置->first.find("\n") == Ustr::npos)
			{
				传入格式化容器.push_back("\n[" + 区域设置->first + "]\n"); //先输出区域设置, 带换行符
			}
			else //有换行符
			{
				传入格式化容器.push_back("\n[" + 区域设置->first + "]");
			}
			//区域内容
			for (auto temp解析后的区域内容 = 区域设置->second.begin(); temp解析后的区域内容 != 区域设置->second.end(); temp解析后的区域内容++)
			{
				if (temp解析后的区域内容->second.find("\n") == Ustr::npos)
				{
					传入格式化容器.push_back(temp解析后的区域内容->first + "=" + temp解析后的区域内容->second + "\n"); //先输出区域设置, 带换行符
				}
				else //有换行符
				{
					传入格式化容器.push_back(temp解析后的区域内容->first + "=" + temp解析后的区域内容->second); //先输出区域设置, 带换行符
				}
			}
		}


		/*if (传入格式化容器.size() > 2)
		{
			for (auto tempStr = 传入格式化容器.begin(); tempStr != 传入格式化容器.end(); tempStr++)
			{
				lg(("格式化: " + *tempStr).c_str(), lgm::wr);
			}
		}*/

		return true;
	}
	else
	{
		return false;
	}
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::写入文件()
{
	if (this->区域总容器.size() > 0) {
		std::vector<Ustr> tempVec写入内容;
		this->格式化(tempVec写入内容);

		std::ofstream 写入文件流(this->配置文件_路径, std::ios::out);
		if (!文件写入(写入文件流, this->配置文件_路径, tempVec写入内容)) {
			return false;
		}

		if (tempVec写入内容.size() > 2)
		{
			for (auto tempBegin = tempVec写入内容.begin(); tempBegin != tempVec写入内容.end(); tempBegin++)
			{
				lgc("写入缓冲字符: " + *tempBegin, lgm::ts);
			}
		}
		//lgc("空行: " + this->空行位置统计.size(), lgm::ts);
		
		return true;
	}

	return false;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::写入文件(std::vector<Ustr>& _文本内容)
{
	std::ofstream 写入文件流(this->配置文件_路径, std::ios::out);
	if (!文件写入(写入文件流, this->配置文件_路径, _文本内容)) {
		return false;
	}

	if (_文本内容.size() > 0)
	{
		for (auto tempBegin = _文本内容.begin(); tempBegin != _文本内容.end(); tempBegin++)
		{
			lgc((Ustr)"写入缓冲字符: " + *tempBegin, lgm::ts);
		}
	}

	return true;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::创建文件()
{
	std::vector<Ustr> temp;
	std::ofstream 写入文件流(this->配置文件_路径, std::ios::out);
	if (!文件写入(写入文件流, this->配置文件_路径, temp)) {
		return false;
	}
	
	return true;
}

bool Typical_Tool::StringHandling::CharHandleOfConfigFile::创建文件(const Ustr& 新文件路径)
{
	std::vector<Ustr> temp;
	std::ofstream 写入文件流(this->配置文件_路径, std::ios::out);
	if (!文件写入(写入文件流, 新文件路径, temp)) {
		return false;
	}
	
	return true;
}

//UTF-8 字符集流程: 源码, 执行, 解析
//Windows 默认根据地区的 ANSI 来确定字符编码
//中文区: ANSI == GBK(GB2312), 其他同理

//源码
//源码字符集: 另存文件为 UTF-8 [MSVC: 高级保存项->Unicode - UTF-8 带签名BOM(不带 Windows可能会识别错误)]

//执行: 写入的文本一定是 utf-8
//#pragma execution_wchar_tacter_set("utf-8") // c++11可以使用 L 标识符
//C++11 支持: wchar_t *utf8[] = "这是中文";

//解析
// 1.
//将程序的locale设置为中国大陆使用的UTF-8编码
//setlocale(LC_ALL, "zh_CN.UTF-8");
//控制台显示乱码纠正
// 2.
//SetConsoleOutputCP(CP_UTF8); //和 chcp 65001同理
//system("chcp 65001"); //设置控制台输出的字符集
// 3.
/*
Ucout << "修改前: ";
Ucout << Ucout.getloc().name() << std::endl;
std::locale locale_wcout("chs"); //locale 设置 Ucout 区域为中文区
Ucout.imbue(locale_wcout);  //设置语言环境
std::wcin.imbue(locale_wcout);  //设置语言环境
Ucout << "修改后: ";
Ucout << Ucout.getloc().name() << std::endl;
*/