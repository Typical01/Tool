//#include "pch.h"

#include "ConfigFileTextManage.h"


using namespace Typical_Tool::StringManage;

Typical_Tool::StringManage::ConfigFileTextManage::~ConfigFileTextManage()
{
}

bool Typical_Tool::StringManage::ConfigFileTextManage::Init(const Tstr& _ConfigFilePath, bool _Analyze)
{
	//保存路径
	this->ConfigFilePath = _ConfigFilePath;

	if (_Analyze) {
		//读取配置文件
		if (!ReadConfigFile()) {
			return false;
		}
		else {
			//解析文本
			if (!this->Analyze()) {
				return false;
			}
		}
	}
	else { //不解析
		if (this->ShowManageLog) {
			lgcr("Init: 不解析文本!\n", lm::ts);
		}
		return false;
	}

	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::InitText(const Tstr& _ConfigFilePath, std::vector<Tstr>& _ReadText)
{
	//保存路径
	this->ConfigFilePath = _ConfigFilePath;

	//读取配置文件
	if (!ReadConfigFile()) {
		lgcr("InitText: 可能没有对应的文件, 或文件正在被使用!\n", lm::wr);
		return false;
	}

	_ReadText = this->TextCache;

	return true;
}

Tstr Typical_Tool::StringManage::ConfigFileTextManage::GetConfigItem_Value(const Tstr& _Config, const Tstr& _ConfigItem)
{
	auto tempConfig = this->ConfigMap.find(_Config);
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgcr("GetConfigItem_Value: 获取配置项 Value\n");
			lgcr("  配置[" + _Config + "]\n");
		}

		auto tempConfigItem = tempConfig->second.find(_ConfigItem);
		if (tempConfigItem != tempConfig->second.end()) {
			if (this->ShowManageLog) {
				lgcr("    配置项[" + _ConfigItem + "]\n");
			}
			return tempConfigItem->second;
		}
		else {
			lgcr("GetConfigItem_Value: 没有找到对应配置项!\n", lm::wr);
			return Tstr();
		}
	}
	else {
		lgcr("GetConfigItem_Value: 没有找到对应配置项!\n", lm::wr);
		return Tstr();
	}
}

std::map<Tstr, Tstr> Typical_Tool::StringManage::ConfigFileTextManage::GetConfigItem(const Tstr& _Config, const Tstr& _ConfigItem)
{
	auto tempConfig = this->ConfigMap.find(_Config);
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgcr("GetConfigItem: 获取配置项\n");
			lgcr("  配置[" + _Config + "]\n");
		}

		auto tempConfigItem = tempConfig->second.find(_ConfigItem);
		if (tempConfigItem != tempConfig->second.end()) {
			if (this->ShowManageLog) {
				lgcr("    配置项[" + _ConfigItem + "]\n");
			}
			
			return { {tempConfigItem->first, tempConfigItem->second} };
		}
		else {
			lgcr("GetConfigItem: 没有找到对应配置项!\n", lm::wr);
			return std::map<Tstr, Tstr>();
		}
	}
	else {
		lgcr("GetConfigItem: 没有找到对应配置项!\n", lm::wr);
		return std::map<Tstr, Tstr>();
	}
}

std::map<Tstr, std::map<Tstr, Tstr>> Typical_Tool::StringManage::ConfigFileTextManage::GetConfig(const Tstr& _Config)
{
	auto tempConfig = this->ConfigMap.find(_Config);
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgcr("GetConfig: 获取配置\n");
			lgcr("  配置[" + _Config + "]\n");
		}

		//返回
		return { {tempConfig->first, tempConfig->second} };
	}
	else {
		lgcr("GetConfig: 没有找到对应配置!\n", lm::wr);
		return std::map<Tstr, std::map<Tstr, Tstr>>();
	}
}

std::map<Tstr, std::map<Tstr, Tstr>> Typical_Tool::StringManage::ConfigFileTextManage::GetConfigMap()
{
	return this->ConfigMap;
}

void Typical_Tool::StringManage::ConfigFileTextManage::OutConfigFile_All() const
{
	lgcr();
	lgcr("OutConfigFile_All: 输出内容 " + this->ConfigFilePath + " 开始...\n", lm::ts);

	for (auto tempConfig = this->ConfigMap.begin(); tempConfig != this->ConfigMap.end(); tempConfig++) {
		lgcr();
		lgcr("  配置 [" + tempConfig->first + "]\n");
		for (auto tempConfigText = tempConfig->second.begin(); tempConfigText != tempConfig->second.end(); tempConfigText++) {
			lgcr("    配置项 Key  [" + tempConfigText->first + "]\n");
			lgcr("    配置项 Value[" + tempConfigText->second + "]\n");
		}
	}
	lgcr();
	lgcr("OutConfigFile_All: 输出内容 " + this->ConfigFilePath + " 结束!\n", lm::ts);
}

bool Typical_Tool::StringManage::ConfigFileTextManage::AddConfig(const Tstr& _Config, std::vector<Tstr>& _ConfigItem)
{
	//先解析为 map
	std::map<Tstr, Tstr> AnalyzeLaterConfigItemMap; //解析后的配置项 Map
	Tstr tempAnalyzeLaterConfigItem_Key; //解析后的配置项 Key
	Tstr tempAnalyzeLaterConfigItem_Value; //解析后的配置项 Value
	for (auto tempConfigItem = _ConfigItem.begin(); tempConfigItem != _ConfigItem.end(); tempConfigItem++) {
		if (this->ConfigItemTextManage(*tempConfigItem, tempAnalyzeLaterConfigItem_Key, tempAnalyzeLaterConfigItem_Value)) {
			lgcr("AddConfig: 配置项文本处理失败!\n", lm::wr);
			return false;
		}
		else { //成功处理后, 临时保存
			AnalyzeLaterConfigItemMap.insert(std::make_pair(tempAnalyzeLaterConfigItem_Key, tempAnalyzeLaterConfigItem_Value));
		}
	}

	std::map<Tstr, Tstr> RepeatConfigItemMap; //重复配置 Map
	//添加有匹配的配置时, 不添加重复配置项
	auto tempConfig = this->ConfigMap.find(_Config);
	if (tempConfig != this->ConfigMap.end()) { //重复配置
		if (this->ShowManageLog) {
			lgcr("AddConfig: 重复配置 [" + _Config + "]添加配置项开始...\n", lm::ts);
		}

		//找重复的配置项
		auto tempConfigItem = tempConfig->second;
		for (auto& ConfigItem : AnalyzeLaterConfigItemMap) {
			auto Result = tempConfigItem.find(ConfigItem.first);
			if (Result != tempConfigItem.end()) { //找到了重复项
				AnalyzeLaterConfigItemMap.erase(ConfigItem.first); //不进入之后的添加流程
			}
		}
	}
	else {
		if (this->ShowManageLog) {
			lgcr("AddConfig: 配置 [" + _Config + "]添加配置项开始...\n", lm::ts);
		}
	}

	//临时存放
	std::map<Tstr, Tstr> tempAddConfigItemMap;
	for (auto& ConfigItem : AnalyzeLaterConfigItemMap) {
		tempAddConfigItemMap.insert(std::make_pair(ConfigItem.first, ConfigItem.second));

		if (this->ShowManageLog) {
			lgcr("    配置项 Key  [" + ConfigItem.first + "]\n");
			lgcr("    配置项 Value[" + ConfigItem.second + "]\n");
		}
	}

	//添加到 总配置
	std::pair<std::map<Tstr, std::map<Tstr, Tstr>>::iterator, bool> tempPair \
		= this->ConfigMap.emplace(std::make_pair(_Config, tempAddConfigItemMap));
	if (!tempPair.second) {
		lgcr("AddConfig: 配置[" + _Config + "] 添加失败!\n", lm::wr);
		return false;
	}
	else {
		if (this->ShowManageLog) {
			lgcr("AddConfig: 配置[" + _Config + "] 添加成功!\n", lm::ts);
		}
	}

	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::AddConfig(const Tstr& _Config, std::map<Tstr, Tstr>& _ConfigItem)
{
	std::map<Tstr, Tstr> RepeatConfigItemMap; //重复配置Map
	//添加有匹配的配置时, 不添加重复配置项
	auto tempConfig = this->ConfigMap.find(_Config); 
	if (tempConfig != this->ConfigMap.end()) { //重复配置
		if (this->ShowManageLog) {
			lgcr("AddConfig: 重复配置 [" + _Config + "]添加配置项开始...\n", lm::ts);
		}

		//找重复的配置项
		auto tempConfigItem = tempConfig->second;
		for (auto& ConfigItem : _ConfigItem) {
			auto Result = tempConfigItem.find(ConfigItem.first);
			if (Result != tempConfigItem.end()) { //找到了重复项
				_ConfigItem.erase(ConfigItem.first); //不进入之后的添加流程
			}
		}
	}
	else {
		if (this->ShowManageLog) {
			lgcr("AddConfig: 配置 [" + _Config + "]添加配置项开始...\n", lm::ts);
		}
	}

	//临时存放
	std::map<Tstr, Tstr> tempAddConfigItemMap;
	for (auto& ConfigItem : _ConfigItem) {
		tempAddConfigItemMap.insert(std::make_pair(ConfigItem.first, ConfigItem.second));

		if (this->ShowManageLog) {
			lgcr("  配置项 Key  [" + ConfigItem.first + "]\n");
			lgcr("  配置项 Value[" + ConfigItem.second + "]\n");
		}
	}

	//添加到 总配置
	std::pair<std::map<Tstr, std::map<Tstr, Tstr>>::iterator, bool> tempPair \
		= this->ConfigMap.emplace(std::make_pair(_Config, tempAddConfigItemMap));
	if (!tempPair.second) {
		lgcr("AddConfig: 配置[" + _Config + "] 添加失败!\n", lm::wr);
		return false;
	}
	else {
		if (this->ShowManageLog) {
			lgcr("AddConfig: 配置[" + _Config + "] 添加成功!\n", lm::ts);
		}
	}

	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::DeleteConfigItem(const Tstr& _Config, const Tstr& _ConfigItem)
{
	//需要删除的配置项是否匹配
	auto temp = this->ConfigMap.find(_Config); //对应配置
	if (temp != this->ConfigMap.end()) {
		if (temp->second.erase(_ConfigItem)) { //删除对应配置项
			if (this->ShowManageLog) {
				lgcr("DeleteConfigItem: 删除配置项\n");
				lgcr("  配置[" + _Config + "]\n");
				lgcr("    配置项[" + _ConfigItem + "]\n");
			}
			return true;
		}
		else {
			lgcr("DeleteConfigItem: 没有找到对应配置项!", lm::wr);
			return false;
		}
	}
	else {
		lgcr("DeleteConfigItem: 没有找到对应配置!", lm::wr);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::DeleteConfig(const Tstr& _Config, bool _IsDelete)
{
	if (_IsDelete) {
		if (this->ConfigMap.erase(_Config)) {  //删除对应配置
			if (this->ShowManageLog) {
				lgcr("DeleteConfig: 删除配置\n");
				lgcr("  配置[" + _Config + "]\n");
			}
			return true;
		}
		else {
			lgcr("DeleteConfig: 没有找到对应配置!", lm::wr);
			return false;
		}
	}
	else {
		lgcr("DeleteConfig: 未进行二次确认[" + _Config + "]\n", lm::wr);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::SetConfigItemValue(const Tstr& _Config, const Tstr& _ConfigItem_Key, const Tstr& _ConfigItem_Value)
{
	auto tempConfig = this->ConfigMap.find(_Config); //对应配置
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgcr("SetConfigItemValue: 修改配置项 Value\n");
			lgcr("  配置[" + _Config + "]\n");
		}
		auto tempConfigItem = tempConfig->second.find(_ConfigItem_Key); //对应配置
		if (tempConfigItem != tempConfig->second.end()) {
			if (this->ShowManageLog) {
				lgcr("    配置项 Key  [" + tempConfigItem->first + "]\n");
				lgcr("    配置项 Value[" + tempConfigItem->second + "]\n");
			}

			tempConfig->second.at(_ConfigItem_Key) = _ConfigItem_Value; //修改配置项

			if (this->ShowManageLog) {
				lgcr("SetConfigItemValue: 修改后...\n", lm::ts);
				lgcr("    配置项 Value[" + _ConfigItem_Value + "]\n");
			}
		}

		return true;
	}
	else {
		lgcr("SetConfigItemValue: 没有找到对应配置项!\n", lm::wr);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::SetConfigItemKey(const Tstr& _Config, const Tstr& _ConfigItem_Key, const Tstr& _ConfigItem_NewKey)
{
	auto tempConfig = this->ConfigMap.find(_Config); //对应配置
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgcr("SetConfigItemKey: 修改配置项 Key\n");
			lgcr("  配置[" + _Config + "]\n");
		}
		auto tempConfigItem = tempConfig->second.find(_ConfigItem_Key); //对应配置
		if (tempConfigItem != tempConfig->second.end()) {
			if (this->ShowManageLog) {
				lgcr("    配置项 Key[" + tempConfigItem->first + "]\n");
				lgcr("    配置项 Value[" + tempConfigItem->second + "]\n");
			}

			auto tempConfigItem_OldValue = tempConfigItem->second;
			//修改配置项
			if (tempConfig->second.erase(_ConfigItem_Key)) { 
				tempConfig->second.insert(std::make_pair(_ConfigItem_NewKey, tempConfigItem_OldValue));

				if (this->ShowManageLog) {
					lgcr("SetConfigItemKey: 修改后...\n", lm::ts);
					lgcr("    配置项 Key[" + _ConfigItem_NewKey + "]\n");
				}
			}
			else {
				lgcr("SetConfigItemKey: 删除旧配置项 Key 失败! 添加新配置项 Key 操作被跳过!\n", lm::wr);
				return false;
			}
		}

		return true;
	}
	else {
		lgcr("SetConfigItemKey: 没有找到对应配置项!\n", lm::wr);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::SetConfig(const Tstr& _Config, const Tstr& _NewConfig)
{
	auto tempConfig = this->ConfigMap.find(_Config); //对应配置
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgcr("SetConfig: 修改配置\n", lm::ts);
			lgcr("  配置[" + _Config + "]\n", lm::ts);
		}

		//保存旧配置的配置项
		auto OldConfigMap = tempConfig->second;

		if (this->ConfigMap.erase(_Config)) { //删除旧配置
			this->ConfigMap.insert(std::make_pair(_NewConfig, OldConfigMap)); //添加新配置和保存的旧配置项

			if (this->ShowManageLog) {
				lgcr("SetConfig: 修改配置[" + _Config + "]成功!\n", lm::ts);
			}
		}
		else {
			lgcr("SetConfig: 删除旧配置[" + _Config + "]失败!\n", lm::wr);
			return false;
		}

		return true;
	}
	else {
		lgcr("SetConfig: 没有找到对应配置项!\n", lm::wr);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::CreateFile(const Tstr& _FileEncode)
{
	if (!WriteFile(this->ConfigFilePath, std::vector<Tstr>(), _FileEncode)) {
		lgcr("CreateFile: 创建文件失败!\n", lm::wr);
		return false;
	}

	lgcr("CreateFile: 创建文件成功!\n", lm::ts);
	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::CreateFile(const Tstr& _NewFilePath, const Tstr& _FileEncode)
{
	if (!WriteFile(_NewFilePath, std::vector<Tstr>(), _FileEncode)) {
		lgcr("CreateFile: 创建文件失败!\n", lm::wr);
		return false;
	}

	lgcr("CreateFile: 创建文件成功!\n", lm::ts);
	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::WriteConfigFile(const Tstr& _FileEncode)
{
	std::vector<Tstr> tempWriteConfig_Vec;
	if (this->Format(tempWriteConfig_Vec)) {
		if (!WriteFile(this->ConfigFilePath, tempWriteConfig_Vec, _FileEncode)) {
			lgcr("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!\n", lm::wr);
			return false;
		}

		if (this->ShowManageLog) {
			if (tempWriteConfig_Vec.size() > 2)
			{
				int tempOtherCharLine_Count = 1;
				lgcr("WriteConfigFile: 写入过程...\n");
				for (auto tempBegin = tempWriteConfig_Vec.begin(); tempBegin != tempWriteConfig_Vec.end(); tempBegin++)
				{
					auto tempOtherChar = this->OtherCharMap.find(tempOtherCharLine_Count);
					if (tempOtherChar != this->OtherCharMap.end()) {
						lgcr("  写入其他字符[" + tempOtherChar->second + "]\n");
						lgcr();
					}
					lgcr("  写入字符[" + *tempBegin + "]\n");
					lgcr();

					tempOtherCharLine_Count++;
				}
			}

			lgcr("WriteConfigFile: 配置写入文件成功!\n", lm::ts);
		}

		return true;
	}
	else {
		lgcr("WriteConfigFile: 格式化配置文本失败!\n", lm::wr);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::WriteTextFile(std::vector<Tstr>& _Text, const Tstr& _FileEncode)
{
	if (!WriteFile(this->ConfigFilePath, _Text, _FileEncode)) {
		lgcr("WriteTextFile: 可能没有对应的文件, 或文件正在被使用!\n", lm::wr);
		return false;
	}
	else {
		if (this->ShowManageLog) {
			for (auto tempBegin = _Text.begin(); tempBegin != _Text.end(); tempBegin++)
			{
				if (this->ShowManageLog) {
					lgcr("  写入字符[" + *tempBegin + "]\n");
				}
			}

		}

		lgcr("WriteTextFile: 文本写入文件成功!\n", lm::ts);
		return true;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::ReadConfigFile(const Tstr& _FileEncode)
{
	//读取配置文件
	if (!ReadFile(this->ConfigFilePath, this->TextCache)) {
		lgcr("ReadConfigFile: 可能没有对应的文件, 或文件正在被使用!\n", lm::wr);
		return false;
	}

	if (this->ShowManageLog) {
		for (auto tempBegin = this->TextCache.begin(); tempBegin != this->TextCache.end(); tempBegin++)
		{
			if (this->ShowManageLog) {
				lgcr("  读取字符[" + *tempBegin + "]\n");
			}
		}

	}
	
	lgcr("WriteTextFile: 文件读取配置成功!\n", lm::ts);
	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::Format(std::vector<Tstr>& _FormatMap)
{
	bool FirstLineText = true; //第一行文本
	int TextLine = 1; //文本行数

	if (this->ConfigMap.size() > 1)
	{
		for (auto tempConfig = this->ConfigMap.begin(); tempConfig != this->ConfigMap.end(); tempConfig++)
		{
			//其他字符
			auto tempOtherChar = this->OtherCharMap.find(TextLine);
			if (tempOtherChar != this->OtherCharMap.end()) {
				_FormatMap.push_back(tempOtherChar->second);
			}
			TextLine++;

			//配置
			if (FirstLineText) { //第一个设置不需要换行
				_FormatMap.push_back("[" + tempConfig->first + "]\n"); //先输出Config, 带换行符
				FirstLineText = false;
			}
			else { //第一个设置之后都使用 换行区分开
				_FormatMap.push_back("\n[" + tempConfig->first + "]\n"); //先输出Config, 带换行符
			}

			//配置项
			for (auto tempConfigItem = tempConfig->second.begin(); tempConfigItem != tempConfig->second.end(); tempConfigItem++)
			{
				if (tempConfigItem->second.find('\n') == Tstr::npos)
				{
					_FormatMap.push_back(tempConfigItem->first + "=" + tempConfigItem->second + "\n"); //先输出Config, 带换行符
				}
				else //有换行符
				{
					_FormatMap.push_back(tempConfigItem->first + "=" + tempConfigItem->second); //先输出Config, 带换行符
				}
			}
		}


		if (this->ShowManageLog) {
			lgcr("Format: 格式化文本输出开始...\n\n", lm::ts);
			for (auto tempFormatText = _FormatMap.begin(); tempFormatText != _FormatMap.end(); tempFormatText++)
			{
				lgcr("  " + *tempFormatText);
			}
			lgcr("\n\nFormat: 格式化文本输出结束!\n", lm::ts);
		}

		return true;
	}
	else
	{
		lgcr("Format: 配置项不足以形成格式!\n", lm::wr);
		ConfigFormatSample();
		return false;
	}
}


bool Typical_Tool::StringManage::ConfigFileTextManage::Analyze()
{
	//配置项非空
	if (!this->TextCache.size() < 2) {
		Tstr AnalyzeLaterConfigItem_Key; //解析后配置项 键
		Tstr AnalyzeLaterConfigItem_Value; //解析后配置项 值
		std::map<Tstr, Tstr> AnalyzeLaterConfigItem; //解析后配置项
		Tstr Config; //配置

		if (this->ShowManageLog) {
			lgcr("Analyze: 解析内容开始...\n");
		}

		/* 配置/配置项 划分 */
		for (int i = 0; i < this->TextCache.size(); i++) {
			Tstr tempAnalyzeText = this->TextCache[i]; //解析的文本
			ConfigStringFormat StrFormat = AnalyzeFormat(tempAnalyzeText); //解析格式

			if (this->ShowManageLog) {
				lgcr("  解析内容[" + tempAnalyzeText + "]\n");
			}

			if (StrFormat != ConfigStringFormat::OtherChar) { //非控制字符
				// 配置
				if (StrFormat == ConfigStringFormat::Config) {
					//配置计数增加
					this->ConfigSum++;

					//是否到下一个Config
					if (this->ConfigSum > 1) {
						//保存到 配置map
						this->ConfigMap.insert(std::make_pair(Config, AnalyzeLaterConfigItem));
						//保存后刷新
						AnalyzeLaterConfigItem.clear();
					}

					//配置格式处理
					if (this->ConfigTextManage(tempAnalyzeText)) {
						Config = tempAnalyzeText;
					}

					//直接跳转到下一次
					continue;
				}
				else { //配置项
					if (this->ConfigSum > 0) {
						//配置项处理
						if (this->ConfigItemTextManage(tempAnalyzeText, AnalyzeLaterConfigItem_Key, AnalyzeLaterConfigItem_Value)) {
							//保存到 配置项map
							AnalyzeLaterConfigItem.insert(std::make_pair(AnalyzeLaterConfigItem_Key, AnalyzeLaterConfigItem_Value));
						}

						//直接跳转到下一次
						continue;
					}
				}
			}
			else if (StrFormat == ConfigStringFormat::OtherChar) {
				//记录 其他字符
				OtherCharMap.insert(std::make_pair(i + 1, tempAnalyzeText));

				if (this->ShowManageLog) {
					lgcr("  其他字符串行: " + Tto_string(i) + "\n");
				}
			}
		}
		//保存到 总容器
		this->ConfigMap.insert(std::make_pair(Config, AnalyzeLaterConfigItem));
		this->TextCache.clear();

		return true;
	}
	else {
		lgcr("Analyze: 传入的配置项不足以形成格式!\n", lm::wr);
		ConfigFormatSample();
		return false;
	}
}

ConfigStringFormat Typical_Tool::StringManage::ConfigFileTextManage::AnalyzeFormat(Tstr& _AnalyzeConfig)
{
	if (!_AnalyzeConfig.empty()) {
		//首字符符号
		Tchar temp区域符号 = _AnalyzeConfig[0];

		//字符串是否非法
		if (temp区域符号 == '[') { //配置
			return ConfigStringFormat::Config;
		}
		else if (temp区域符号 >= 0 && temp区域符号 <= 31) { //控制字符: 0~31
			return ConfigStringFormat::OtherChar;
		}
		else if (temp区域符号 == 127) { //控制字符
			return ConfigStringFormat::OtherChar;
		}
		else { //其他字符: 英文/中文/...
			return ConfigStringFormat::ConfigItem;
		}
	}
	else {
		lgcr("AnalyzeFormat: 传入为空字符串!\n", lm::wr);
		return ConfigStringFormat::OtherChar;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::ConfigTextManage(Tstr& _Config)
{
	//格式是否正确
	int tempConfigFormat_Right = _Config.find(']'); //右括号
	if (tempConfigFormat_Right == Tstr::npos) {
		lgcr("ConfigTextManage: 格式错误!\n", lm::wr);
		lgcr("ConfigTextManage: 配置文本[" + _Config + "]\n", lm::wr);

		return false;
	}

	//索引 转换为 个数会比实际数量少1
	//例: 索引为 3, 初始化 string(3, 'a') 为3个字符, 而索引 3在数组中为 第 4位
	//	string(3(索引) + 1(实际还需要), 'a'), string.size() = 4
	//需要去掉 '[', ']' 两个字符: -1

	Tstr Config(tempConfigFormat_Right - 1, ' ');
	//保存配置
	for (int temp_Int = 0;
		temp_Int < tempConfigFormat_Right - 1; ++temp_Int) { //0~3: 4个数
		Config[temp_Int] = _Config[temp_Int + 1];
	}

	if (this->ShowManageLog) {
		lgcr("  配置[" + Config + "]\n");
	}
	_Config = Config;

	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::ConfigItemTextManage(Tstr& _ConfigItem, Tstr& _AnalyzeLaterConfigItem_Key, Tstr& _AnalyzeLaterConfigItem_Value)
{
	//格式是否正确
	int tempEqualSign = _ConfigItem.find('='); //找到 "=" 号的下标
	if (tempEqualSign == Tstr::npos) {
		lgcr("ConfigItemTextManage: 格式错误!", lm::wr);
		lgcr("ConfigItemTextManage: 配置项[" + _ConfigItem + "]\n", lm::wr);
		return false;
	}

	int KeyLength = tempEqualSign; // T: 1
	int MaxIndex = _ConfigItem.length() - 1; //元素个数比下标大 1: length(T=abc\n): 6 - 1 = 5
	//去掉 '\n'符/'='号,后剩余的元素个数;
	// 5 - 2('\n' | '=') = 3
	int ValueLength = MaxIndex - 2; // -2: -'\n' -'='

	Tstr tempKey(KeyLength, ' '); //初始化: 1

	//保存 键
	for (int i = 0; i < KeyLength; i++) {
		tempKey[i] = _ConfigItem[i];
	}

	//保存 值
	// 内容从'='之后开始: T=a 长度3
	// 下标 = 长度 - 1
	// 下标 = 3 - 1 = 2(0, 1, 2; 三个下标)
	// for (5 -'\n' -'=') = for(3)
	Tstr tempValue(ValueLength, ' '); //初始化: 3
	for (int i = KeyLength + 1; i < MaxIndex; i++) {  // -1: -'\n'; 减去换行符;
		//从0开始保存
		tempValue[i - (KeyLength + 1)] = _ConfigItem[i];
	}

	//找到 tempValue后的 '\n', 并删除: 避免使用异常
	if (tempValue.find('\n') != Tstr::npos) {
		tempValue.erase(tempValue.size() - 1);
		if (this->ShowManageLog) {
			lgcr("  删除回车[" + tempValue + "]\n");
		}
	}

	_AnalyzeLaterConfigItem_Key = tempKey;
	_AnalyzeLaterConfigItem_Value = tempValue;

	return true;
}

void Typical_Tool::StringManage::ConfigFileTextManage::ConfigFormatSample()
{
	lgcr();
	lgcr("ConfigFormatSample: 配置格式 示例开始...\n", lm::ts);
	lgcr("  ConfigFileName: cfg.ini\n");
	lgcr("  ConfigItem: \n    config1(key1=value1, key2=value2)\n    config2(key3=value3)\n");
	lgcr("cfg.ini: 下面是实际的文本内容\n");
	lgcr("  [config1]\n");
	lgcr("  key1=value1\n");
	lgcr("  key2=value2\n");
	lgcr("  [config2]\n");
	lgcr("  key3=value3\n");
	lgcr("配置格式 示例结束!\n", lm::ts);
	lgcr();
}


bool Typical_Tool::StringManage::WriteFile(
	const Tstr& _ConfigFilePath,
	std::vector<Tstr>& _WriteText,
	const Tstr& _FileEncode)
{
	try {
		//std::ofstream WriteFileStream = std::move(_FileStream_Out);
		std::ofstream WriteFileStream(_ConfigFilePath, std::ios::out);

		if (!WriteFileStream) {
			lgcr("WriteFile: [" + _ConfigFilePath + "] 打开文件失败!\n", lm::wr);
			return false;
		}
		lgcr("WriteFile: [" + _ConfigFilePath + "] 打开文件成功!\n", lm::ts);
		lgcr("WriteFile: 开始写入!\n");

		if (_FileEncode == "UTF-8BOM") {
			// 写入 UTF-8 BOM (EF BB BF)
			lgcr("WriteFile: 写入 UTF-8BOM编码(EF BB BF).\n");

			WriteFileStream.put(0xEF);
			WriteFileStream.put(0xBB);
			WriteFileStream.put(0xBF);
		}
		else if (_FileEncode == "UTF-8") {
			lgcr("WriteFile: 写入 UTF-8编码.\n");
		}

		for (const Tstr& tempStr : _WriteText) {
			WriteFileStream << tempStr;
		}

		return true;
	}
	catch (...) {
		lgcr("bool Typical_Tool::StringManage::WriteFile()\n", lm::er);
	}
}

bool Typical_Tool::StringManage::ReadFile(
	const Tstr& _ConfigFilePath,
	std::vector<Tstr>& _ReadText,
	const Tstr& _FileEncode)
{
	try {
		//std::ifstream FileStream_In = std::move(_FileStream_In);
		std::ifstream ReadFileStream(_ConfigFilePath, std::ios::binary);

		if (!ReadFileStream) {
			lgcr("ReadFile: [" + _ConfigFilePath + "] 打开文件失败!\n", lm::wr);
			return false;
		}
		lgcr("ReadFile: [" + _ConfigFilePath + "] 打开文件成功!\n", lm::ts);
		lgcr("ReadFile: 开始读取!\n");

		// 读取 BOM（如果存在）
		if (_FileEncode == "UTF-8") {
			std::vector<unsigned char> bom(3);
			ReadFileStream.read(reinterpret_cast<char*>(bom.data()), 3);

			// 检查 BOM 是否为 UTF-8 BOM (EF BB BF)
			if (bom.size() == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
				lgcr("ReadFile: 检测到UTF-8 BOM.\n", lm::ts);
			}
			else {
				// 如果文件没有 BOM，返回文件流的读位置
				ReadFileStream.clear(); // 清除 EOF 标志
				ReadFileStream.seekg(0);
			}
		}

		std::string line;
		//获取行
		while (std::getline(ReadFileStream, line)) {
			_ReadText.push_back(line); //添加到 _Text
		}

		return true;
	}
	catch (...) {
		lgcr("bool Typical_Tool::StringManage::ReadFile()\n", lm::er);
	}
}