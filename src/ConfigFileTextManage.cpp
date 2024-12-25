//#include "pch.h"

#include "ConfigFileTextManage.h"


using namespace Typical_Tool::StringManage;

Typical_Tool::StringManage::ConfigFileTextManage::~ConfigFileTextManage()
{
}

bool Typical_Tool::StringManage::ConfigFileTextManage::Init(const Tstr& _ConfigFilePath, bool _Analyze)
{
	this->ConfigFilePath = _ConfigFilePath; //保存路径

	if (_Analyze) {
		if (!ReadConfigFile()) { //读取配置文件
			lgc("Init: 读取配置文件失败!", lm::war);
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
			lgc("Init: 不解析文本!", lm::tips);
		}
		return false;
	}

	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::InitText(const Tstr& _ConfigFilePath, std::vector<Tstr>& _ReadText)
{
	this->ConfigFilePath = _ConfigFilePath; //保存路径
	
	if (!ReadConfigFile(_ReadText)) { //读取配置文件
		lgc("InitText: 读取配置文件失败!", lm::war);
		return false;
	}

	return true;
}

void Typical_Tool::StringManage::ConfigFileTextManage::SetShowManageLog(bool _IsShowManageLog)
{
	this->ShowManageLog = _IsShowManageLog;
}

Tstr Typical_Tool::StringManage::ConfigFileTextManage::GetConfigItem_Value(const Tstr& _Config, const Tstr& _ConfigItem)
{
	auto tempConfig = this->ConfigMap.find(_Config);
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgc("GetConfigItem_Value: 获取配置项 Value");
			lgc("  配置[" + _Config + "]");
		}

		auto tempConfigItem = tempConfig->second.find(_ConfigItem);
		if (tempConfigItem != tempConfig->second.end()) {
			if (this->ShowManageLog) {
				lgc("    配置项 Key  [" + _ConfigItem + "]");
				lgc("    配置项 Value[" + tempConfigItem->second + "]");
			}
			return tempConfigItem->second;
		}
		else {
			lgc("GetConfigItem_Value: 没有找到对应配置项!", lm::war);
			return Tstr();
		}
	}
	else {
		lgc("GetConfigItem_Value: 没有找到对应配置项!", lm::war);
		return Tstr();
	}
}

std::map<Tstr, Tstr> Typical_Tool::StringManage::ConfigFileTextManage::GetConfigItem(const Tstr& _Config, const Tstr& _ConfigItem)
{
	auto tempConfig = this->ConfigMap.find(_Config);
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgc("GetConfigItem: 获取配置项");
			lgc("  配置[" + _Config + "]");
		}

		auto tempConfigItem = tempConfig->second.find(_ConfigItem);
		if (tempConfigItem != tempConfig->second.end()) {
			if (this->ShowManageLog) {
				lgc("    配置项[" + _ConfigItem + "]");
			}
			
			return { {tempConfigItem->first, tempConfigItem->second} };
		}
		else {
			lgc("GetConfigItem: 没有找到对应配置项!", lm::war);
			return std::map<Tstr, Tstr>();
		}
	}
	else {
		lgc("GetConfigItem: 没有找到对应配置项!", lm::war);
		return std::map<Tstr, Tstr>();
	}
}

std::map<Tstr, Tstr> Typical_Tool::StringManage::ConfigFileTextManage::GetConfig(const Tstr& _Config)
{
	auto tempConfig = this->ConfigMap.find(_Config);
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgc("GetConfig: 获取配置");
			lgc("  配置[" + _Config + "]");
		}

		auto tempConfigItemMap = tempConfig->second;

		//返回
		return std::map<Tstr, Tstr>(tempConfigItemMap.begin(), tempConfigItemMap.end());
	}
	else {
		lgc("GetConfig: 没有找到对应配置!", lm::war);
		return std::map<Tstr, Tstr>();
	}
}

std::map<Tstr, std::map<Tstr, Tstr>> Typical_Tool::StringManage::ConfigFileTextManage::GetConfigMap()
{
	return this->ConfigMap;
}

void Typical_Tool::StringManage::ConfigFileTextManage::OutConfigFile_All() const
{
	lgc("OutConfigFile_All: 输出内容 " + this->ConfigFilePath + " 开始...", lm::tips);

	for (auto tempConfig = this->ConfigMap.begin(); tempConfig != this->ConfigMap.end(); tempConfig++) {
		lgc("  配置[" + tempConfig->first + "]");
		for (auto tempConfigText = tempConfig->second.begin(); tempConfigText != tempConfig->second.end(); tempConfigText++) {
			lgc("    配置项 Key  : " + tempConfigText->first);
			lgc("    配置项 Value: " + tempConfigText->second);
		}
	}
	lgc("OutConfigFile_All: 输出内容 " + this->ConfigFilePath + " 结束!\n", lm::tips);
}

bool Typical_Tool::StringManage::ConfigFileTextManage::AddConfig(const Tstr& _Config, std::vector<Tstr>& _ConfigItem)
{
	//先解析为 map
	std::map<Tstr, Tstr> AnalyzeLaterConfigItemMap; //解析后的配置项 Map
	Tstr tempAnalyzeLaterConfigItem_Key; //解析后的配置项 Key
	Tstr tempAnalyzeLaterConfigItem_Value; //解析后的配置项 Value
	for (auto tempConfigItem = _ConfigItem.begin(); tempConfigItem != _ConfigItem.end(); tempConfigItem++) {
		if (!this->ConfigItemTextManage(*tempConfigItem, tempAnalyzeLaterConfigItem_Key, tempAnalyzeLaterConfigItem_Value)) {
			lgc("AddConfig: 配置项文本处理失败!", lm::war);
			lgc("  配置[" + _Config + "]", lm::war);
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
			lgc("AddConfig: 重复配置[" + _Config + "]添加配置项开始...", lm::tips);
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
			lgc("AddConfig: 配置[" + _Config + "]添加配置项开始...", lm::tips);
		}
	}

	//临时存放
	std::map<Tstr, Tstr> tempAddConfigItemMap;
	for (auto& ConfigItem : AnalyzeLaterConfigItemMap) {
		tempAddConfigItemMap.insert(std::make_pair(ConfigItem.first, ConfigItem.second));

		if (this->ShowManageLog) {
			lgc("    配置项 Key  [" + ConfigItem.first + "]");
			lgc("    配置项 Value[" + ConfigItem.second + "]");
		}
	}

	//添加到 总配置
	std::pair<std::map<Tstr, std::map<Tstr, Tstr>>::iterator, bool> tempPair \
		= this->ConfigMap.emplace(std::make_pair(_Config, tempAddConfigItemMap));
	if (!tempPair.second) {
		lgc("AddConfig: 配置[" + _Config + "]添加失败!", lm::war);
		return false;
	}
	else {
		lgc("AddConfig: 配置[" + _Config + "]添加成功!", lm::tips);
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
			lgc("AddConfig: 重复配置[" + _Config + "]添加配置项开始...", lm::tips);
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
			lgc("AddConfig: 配置[" + _Config + "]添加配置项开始...", lm::tips);
		}
	}

	//临时存放
	std::map<Tstr, Tstr> tempAddConfigItemMap;
	for (auto& ConfigItem : _ConfigItem) {
		tempAddConfigItemMap.insert(std::make_pair(ConfigItem.first, ConfigItem.second));

		if (this->ShowManageLog) {
			lgc("  配置项 Key  [" + ConfigItem.first + "]");
			lgc("  配置项 Value[" + ConfigItem.second + "]");
		}
	}

	//添加到 总配置
	std::pair<std::map<Tstr, std::map<Tstr, Tstr>>::iterator, bool> tempPair \
		= this->ConfigMap.emplace(std::make_pair(_Config, tempAddConfigItemMap));
	if (!tempPair.second) {
		lgc("AddConfig: 配置[" + _Config + "] 添加失败!", lm::war);
		return false;
	}
	else {
		lgc("AddConfig: 配置[" + _Config + "] 添加成功!", lm::tips);
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
				lgc("DeleteConfigItem: 删除配置项");
				lgc("  配置[" + _Config + "]");
				lgc("    配置项[" + _ConfigItem + "]");
			}
			return true;
		}
		else {
			lgc("DeleteConfigItem: 没有找到对应配置项!", lm::war);
			return false;
		}
	}
	else {
		lgc("DeleteConfigItem: 没有找到对应配置!", lm::war);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::DeleteConfig(const Tstr& _Config, bool _IsDelete)
{
	if (_IsDelete) {
		if (this->ConfigMap.erase(_Config)) {  //删除对应配置
			if (this->ShowManageLog) {
				lgc("DeleteConfig: 删除配置");
				lgc("  配置[" + _Config + "]");
			}
			return true;
		}
		else {
			lgc("DeleteConfig: 没有找到对应配置!", lm::war);
			return false;
		}
	}
	else {
		lgc("DeleteConfig: 未进行二次确认[" + _Config + "]", lm::war);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::SetConfigItemValue(const Tstr& _Config, const Tstr& _ConfigItem_Key, const Tstr& _ConfigItem_Value)
{
	auto tempConfig = this->ConfigMap.find(_Config); //对应配置
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgc("SetConfigItemValue: 修改配置项 Value");
			lgc("  配置[" + _Config + "]");
		}
		auto tempConfigItem = tempConfig->second.find(_ConfigItem_Key); //对应配置
		if (tempConfigItem != tempConfig->second.end()) {
			if (this->ShowManageLog) {
				lgc("    配置项 Key  [" + tempConfigItem->first + "]");
				lgc("    配置项 Value[" + tempConfigItem->second + "]");
			}

			tempConfig->second.at(_ConfigItem_Key) = _ConfigItem_Value; //修改配置项

			if (this->ShowManageLog) {
				lgc("SetConfigItemValue: 修改后...", lm::tips);
				lgc("    配置项 Value[" + _ConfigItem_Value + "]");
			}
		}

		return true;
	}
	else {
		lgc("SetConfigItemValue: 没有找到对应配置项!", lm::war);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::SetConfigItemKey(const Tstr& _Config, const Tstr& _ConfigItem_Key, const Tstr& _ConfigItem_NewKey)
{
	auto tempConfig = this->ConfigMap.find(_Config); //对应配置
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgc("SetConfigItemKey: 修改配置项 Key");
			lgc("  配置[" + _Config + "]");
		}
		auto tempConfigItem = tempConfig->second.find(_ConfigItem_Key); //对应配置
		if (tempConfigItem != tempConfig->second.end()) {
			if (this->ShowManageLog) {
				lgc("    配置项 Key  [" + tempConfigItem->first + "]");
				lgc("    配置项 Value[" + tempConfigItem->second + "]");
			}

			auto tempConfigItem_OldValue = tempConfigItem->second;
			//修改配置项
			if (tempConfig->second.erase(_ConfigItem_Key)) { 
				tempConfig->second.insert(std::make_pair(_ConfigItem_NewKey, tempConfigItem_OldValue));

				if (this->ShowManageLog) {
					lgc("SetConfigItemKey: 修改后...", lm::tips);
					lgc("    配置项 Key[" + _ConfigItem_NewKey + "]");
				}
			}
			else {
				lgc("SetConfigItemKey: 删除旧配置项 Key 失败! 添加新配置项 Key 操作被跳过!", lm::war);
				return false;
			}
		}

		return true;
	}
	else {
		lgc("SetConfigItemKey: 没有找到对应配置项!", lm::war);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::SetConfig(const Tstr& _Config, const Tstr& _NewConfig)
{
	auto tempConfig = this->ConfigMap.find(_Config); //对应配置
	if (tempConfig != this->ConfigMap.end()) {
		if (this->ShowManageLog) {
			lgc("SetConfig: 修改配置", lm::tips);
			lgc("  配置[" + _Config + "]", lm::tips);
		}

		//保存旧配置的配置项
		auto OldConfigMap = tempConfig->second;

		if (this->ConfigMap.erase(_Config)) { //删除旧配置
			this->ConfigMap.insert(std::make_pair(_NewConfig, OldConfigMap)); //添加新配置和保存的旧配置项

			if (this->ShowManageLog) {
				lgc("SetConfig: 修改配置[" + _Config + "]成功!", lm::tips);
			}
		}
		else {
			lgc("SetConfig: 删除旧配置[" + _Config + "]失败!", lm::war);
			return false;
		}

		return true;
	}
	else {
		lgc("SetConfig: 没有找到对应配置项!", lm::war);
		return false;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::CreateFile(const Tstr& _FileEncode)
{
	if (!WriteFile(this->ConfigFilePath, std::vector<Tstr>(), _FileEncode)) {
		lgc("CreateFile: 创建文件失败!", lm::war);
		return false;
	}

	lgc("CreateFile: 创建文件成功!", lm::tips);
	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::CreateFile(const Tstr& _NewFilePath, const Tstr& _FileEncode)
{
	if (!WriteFile(_NewFilePath, std::vector<Tstr>(), _FileEncode)) {
		lgc("CreateFile: 创建文件失败!", lm::war);
		return false;
	}

	lgc("CreateFile: 创建文件成功!", lm::tips);
	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::WriteConfigFile(bool _IsSureWrite, const Tstr& _FileEncode)
{
	if (!this->FormatText) {
		if (!_IsSureWrite) { 
			std::vector<Tstr> tempWriteConfig_Vec;
			if (this->Format(tempWriteConfig_Vec)) {
				if (!WriteFile(this->ConfigFilePath, tempWriteConfig_Vec, _FileEncode)) {
					lgc("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!", lm::war);
					return false;
				}

				if (this->ShowManageLog) {
					if (tempWriteConfig_Vec.size() > 2)
					{
						int tempOtherCharLine_Count = 1;
						lgc("WriteConfigFile: 写入过程...");
						for (auto tempBegin = tempWriteConfig_Vec.begin(); tempBegin != tempWriteConfig_Vec.end(); tempBegin++)
						{
							auto tempOtherChar = this->OtherCharMap.find(tempOtherCharLine_Count);
							if (tempOtherChar != this->OtherCharMap.end()) {
								lgc("  写入其他字符: " + tempOtherChar->second, lm::txt);
							}
							lgc("  写入字符: " + *tempBegin, lm::txt);

							tempOtherCharLine_Count++;
						}
					}

					lgc("WriteConfigFile: 配置写入文件成功!", lm::tips);
				}

				this->FormatText = true; //已经格式化
				this->TextCache = tempWriteConfig_Vec;

				return true;
			}
			else {
				lgc("WriteConfigFile: 格式化配置文本失败!", lm::war);
				return false;
			}
		}
		else { //强制写入
			std::vector<Tstr> tempWriteConfig_Vec;
			this->Format(tempWriteConfig_Vec); //无视格式化是否成功
			if (!WriteFile(this->ConfigFilePath, tempWriteConfig_Vec, _FileEncode)) {
				lgc("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!", lm::war);
				return false;
			}

			if (this->ShowManageLog) {
				if (tempWriteConfig_Vec.size() > 2)
				{
					int tempOtherCharLine_Count = 1;
					lgc("WriteConfigFile: 写入过程...");
					for (auto tempBegin = tempWriteConfig_Vec.begin(); tempBegin != tempWriteConfig_Vec.end(); tempBegin++)
					{
						auto tempOtherChar = this->OtherCharMap.find(tempOtherCharLine_Count);
						if (tempOtherChar != this->OtherCharMap.end()) {
							lgc("  写入其他字符: " + tempOtherChar->second, lm::txt);
						}
						lgc("  写入字符: " + *tempBegin, lm::txt);

						tempOtherCharLine_Count++;
					}
				}

				lgc("WriteConfigFile: 配置写入文件成功!", lm::tips);
			}

			this->FormatText = true; //已经格式化
			this->TextCache = tempWriteConfig_Vec;

			return true;
		}
	}
	else {
		if (this->ShowManageLog) {
			lgc("WriteConfigFile: 已格式化配置文本!", lm::tips);
		}

		if (!WriteFile(this->ConfigFilePath, this->TextCache, _FileEncode)) {
			lgcr("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!", lm::war);
			return false;
		}

		return true;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::WriteTextFile(std::vector<Tstr>& _Text, const Tstr& _FileEncode)
{
	if (!WriteFile(this->ConfigFilePath, _Text, _FileEncode)) {
		lgc("WriteTextFile: 可能没有对应的文件, 或文件正在被使用!", lm::war);
		return false;
	}
	else {
		if (this->ShowManageLog) {
			for (auto tempBegin = _Text.begin(); tempBegin != _Text.end(); tempBegin++)
			{
				if (this->ShowManageLog) {
					lgc("  写入字符: " + *tempBegin);
				}
			}

		}

		lgc("WriteTextFile: 文本写入文件成功!", lm::tips);
		return true;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::ReadConfigFile(const Tstr& _FileEncode)
{
	//读取配置文件
	if (!ReadFile(this->ConfigFilePath, this->TextCache)) {
		lgc("ReadConfigFile: 可能没有对应的文件, 或文件正在被使用!", lm::war);
		return false;
	}

	if (this->ShowManageLog) {
		for (auto tempBegin = this->TextCache.begin(); tempBegin != this->TextCache.end(); tempBegin++)
		{
			lgc("  读取字符: " + *tempBegin, lm::txt);
			lgc();
		}

	}
	
	lgc("ReadConfigFile: 文件读取配置成功!", lm::tips);
	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::ReadConfigFile(std::vector<Tstr>& _Text, const Tstr& _FileEncode)
{//读取配置文件
	if (!ReadFile(this->ConfigFilePath, _Text)) {
		lgc("ReadConfigFile: 可能没有对应的文件, 或文件正在被使用!", lm::war);
		lgc();
		return false;
	}

	if (this->ShowManageLog) {
		for (auto tempBegin = _Text.begin(); tempBegin != _Text.end(); tempBegin++)
		{
			lgc("  读取字符: " + *tempBegin, lm::txt);
		}

	}

	lgc("ReadConfigFile: 文件读取配置成功!", lm::tips);
	return true;
	return false;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::Format(std::vector<Tstr>& _FormatMap)
{
	bool FirstLineText = true; //第一行文本
	int TextLine = 1; //文本行数

	//示例文本: 总是在配置文件的开头
	auto tempFormatSampleText = this->OtherCharMap.find(0);
	if (tempFormatSampleText != this->OtherCharMap.end()) {
		_FormatMap.push_back(tempFormatSampleText->second);
	}

	if (!this->ConfigMap.empty())
	{
		if (this->ShowManageLog) {
			lgc("Format: 格式化文本输出开始...", lm::tips);
		}
		for (auto tempConfig = this->ConfigMap.begin(); tempConfig != this->ConfigMap.end(); tempConfig++)
		{
			//其他字符
			auto tempOtherChar = this->OtherCharMap.find(TextLine);
			if (tempOtherChar != this->OtherCharMap.end()) {
				_FormatMap.push_back(tempOtherChar->second);
			}
			TextLine++;

			//配置
			auto tempConfigText = "[" + tempConfig->first + "]";
			if (this->ShowManageLog) {
				lgc(tempConfigText);
			}
			if (FirstLineText) { //第一个配置不需要
				_FormatMap.push_back(tempConfigText + "\n"); //带换行符
				FirstLineText = false;
			}
			else { //每一个配置中间使用换行隔开
				_FormatMap.push_back("\n");
				_FormatMap.push_back(tempConfigText + "\n"); //带换行符
			}

			//配置项
			for (auto tempConfigItem = tempConfig->second.begin(); tempConfigItem != tempConfig->second.end(); tempConfigItem++)
			{
				auto tempText = tempConfigItem->first + "=" + tempConfigItem->second;
				if (this->ShowManageLog) {
					lgc(tempText);
				}
				if (tempConfigItem->second.find('\n') == Tstr::npos)
				{
					_FormatMap.push_back(tempText + "\n"); //带换行符
				}
				else //有换行符
				{
					_FormatMap.push_back(tempText);
				}
			}
		}
		if (this->ShowManageLog) {
			lgc("Format: 格式化文本输出结束!", lm::tips);
		}

		return true;
	}
	else //空配置
	{
		_FormatMap.push_back("");

		lgc("Format: 配置项不足以形成格式!", lm::war);
		ConfigFormatSample();
		return false;
	}
}


bool Typical_Tool::StringManage::ConfigFileTextManage::Analyze()
{
	//配置项非空
	if (!(this->TextCache.size() < 2)) {
		Tstr AnalyzeLaterConfigItem_Key; //解析后配置项 键
		Tstr AnalyzeLaterConfigItem_Value; //解析后配置项 值
		std::map<Tstr, Tstr> AnalyzeLaterConfigItem; //解析后配置项
		Tstr NewConfig; //新的配置: 永远是最新的
		Tstr OldConfig; //旧的配置

		if (this->ShowManageLog) {
			lgc("Analyze: 解析内容开始...", lm::tips);
		}

		/* 配置/配置项 划分 */
		for (int i = 0; i < this->TextCache.size(); i++) {
			Tstr tempAnalyzeText = this->TextCache[i]; //解析的文本
			if (this->ShowManageLog) {
				lgc("解析内容: " + tempAnalyzeText);
			}

			ConfigStringFormat StrFormat = AnalyzeFormat(tempAnalyzeText); //解析格式
			//非控制字符

			if (StrFormat == ConfigStringFormat::Config) { // 配置

				//是否到下一个Config
				if (OldConfig != NewConfig) {
					if (!AnalyzeLaterConfigItem.empty()) {
						this->ConfigMap.insert(std::make_pair(NewConfig, AnalyzeLaterConfigItem)); //保存到 配置map
						OldConfig = NewConfig; //新的配置被保存, 变成旧的配置
						AnalyzeLaterConfigItem.clear(); //保存后刷新
						NewConfig.clear();
					}
				}

				//配置格式处理
				if (this->ConfigTextManage(tempAnalyzeText)) {
					NewConfig = tempAnalyzeText;
					if (this->ShowManageLog) {
						lgc("  配置[" + tempAnalyzeText + "]");
					}
				}
			}
			else if (StrFormat == ConfigStringFormat::ConfigItem) { //配置项
				if (!NewConfig.empty()) { //有配置
					//配置项处理
					if (this->ConfigItemTextManage(tempAnalyzeText, AnalyzeLaterConfigItem_Key, AnalyzeLaterConfigItem_Value)) {
						if (AnalyzeLaterConfigItem_Key.empty()) { //Key为空
							lgc("Analyze: 传入的配置项[Key]格式有误!", lm::war);
							lgc("    Key: " + AnalyzeLaterConfigItem_Key, lm::war);
						}
						if (AnalyzeLaterConfigItem_Value.empty()) { //Value为空
							lgc("Analyze: 传入的配置项[Value]格式有误!", lm::war);
							lgc("    Value: " + AnalyzeLaterConfigItem_Value, lm::war);
						}

						AnalyzeLaterConfigItem.insert(std::make_pair(AnalyzeLaterConfigItem_Key, AnalyzeLaterConfigItem_Value)); //保存到 配置项map
						if (this->ShowManageLog) {
							lgc("    配置项 Key  [" + AnalyzeLaterConfigItem_Key + "]");
							lgc("    配置项 Value[" + AnalyzeLaterConfigItem_Value + "]");
						}
					}
				}
				else {
					//记录 其他字符
					OtherCharMap.insert(std::make_pair(i + 1, tempAnalyzeText));

					if (this->ShowManageLog) {
						lgc("* 其他字符串行: " + To_string(i) + "");
					}
				}
			}
			else if (StrFormat == ConfigStringFormat::OtherChar) {
				//记录 其他字符
				OtherCharMap.insert(std::make_pair(i + 1, tempAnalyzeText));

				if (this->ShowManageLog) {
					lgc("* 其他字符串行: " + To_string(i) + "");
				}
			}
		}
		//最后一个配置
		this->ConfigMap.insert(std::make_pair(NewConfig, AnalyzeLaterConfigItem)); //保存到 配置map
		this->TextCache.clear();

		if (this->ShowManageLog) {
			lgc("Analyze: 解析完成!", lm::tips);
		}

		return true;
	}
	else {
		lgc("Analyze: 传入的配置项不足以形成格式!", lm::war);
		ConfigFormatSample();
		return false;
	}
}

ConfigStringFormat Typical_Tool::StringManage::ConfigFileTextManage::AnalyzeFormat(Tstr& _AnalyzeConfig)
{
	if (!_AnalyzeConfig.empty()) {
		//首字符符号
		Tchar tempConfigChar = _AnalyzeConfig[0];

		//字符串是否非法
		if (tempConfigChar == '[') { //配置
			return ConfigStringFormat::Config;
		}
		else if (tempConfigChar >= 0 && tempConfigChar <= 31) { //控制字符: 0~31
			if (this->ShowManageLog) {
				lgc("AnalyzeFormat: OtherChar[" + _AnalyzeConfig + "]", lm::war);
			}
			return ConfigStringFormat::OtherChar;
		}
		else if (tempConfigChar == 127) { //控制字符
			if (this->ShowManageLog) {
				lgc("AnalyzeFormat: OtherChar[" + _AnalyzeConfig + "]", lm::war);
			}
			return ConfigStringFormat::OtherChar;
		}
		else { //其他字符: 英文/中文/...
			return ConfigStringFormat::ConfigItem;
		}
	}
	else {
		lgc("AnalyzeFormat: 传入为空字符串!", lm::war);
		return ConfigStringFormat::OtherChar;
	}
}

bool Typical_Tool::StringManage::ConfigFileTextManage::ConfigTextManage(Tstr& _Config)
{
	//格式是否正确
	size_t tempConfigFormat_Right = _Config.find(']'); //右括号
	if (tempConfigFormat_Right == Tstr::npos) {
		lgc("ConfigTextManage: 格式错误!", lm::war);
		lgc("ConfigTextManage: 配置文本[" + _Config + "]", lm::war);

		return false;
	}

	//保存配置
	Tstr tempConfig(_Config.begin() + 1, _Config.end() - 2);

	_Config = tempConfig;

	return true;
}

bool Typical_Tool::StringManage::ConfigFileTextManage::ConfigItemTextManage(Tstr& _ConfigItem, Tstr& _AnalyzeLaterConfigItem_Key, Tstr& _AnalyzeLaterConfigItem_Value)
{
	//格式是否正确
	size_t tempEqualSign = _ConfigItem.find('='); //找到 "=" 号的下标
	if (tempEqualSign == Tstr::npos) {
		lgc("ConfigItemTextManage: 格式错误!", lm::war);
		lgc("ConfigItemTextManage: 配置项[" + _ConfigItem + "]", lm::war);
		return false;
	}

	// Windows 换行处理为: \r\n
	// Unix 换行处理为: \n
	
	//删除文本中的换行符 '\n'
#ifdef _WINDOWS
	if (*(_ConfigItem.end() - 1) == '\n' || *(_ConfigItem.end() - 1) == '\r') { //Windows 找到 '\n' || '\r'
		_ConfigItem.erase(_ConfigItem.end() - 1, _ConfigItem.end());
		if (this->ShowManageLog) {
			lgc("ConfigItemTextManage: 删除换行", lm::tips);
			lgc("    Text[" + _ConfigItem + "]");
		}
	}
#else
	if (*(_ConfigItem.end() - 1) == '\n') { //Unix 找到 '\n'
		_ConfigItem.erase(_ConfigItem.end() - 1, _ConfigItem.end());
		if (this->ShowManageLog) {
			lgc("ConfigItemTextManage: 删除换行", lm::tips);
			lgc("    Text[" + _ConfigItem + "]");
		}
	}
#endif

	//保存 键
	Tstr tempKey(_ConfigItem.begin(), _ConfigItem.begin() + tempEqualSign); //初始化: 等于号之前
	//保存 值
	Tstr tempValue(_ConfigItem.begin() + tempEqualSign + 1, _ConfigItem.end()); //初始化: 等于号之后

	_AnalyzeLaterConfigItem_Key = tempKey;
	_AnalyzeLaterConfigItem_Value = tempValue;

	return true;
}

void Typical_Tool::StringManage::ConfigFileTextManage::ConfigFormatSample()
{
	lgc("ConfigFormatSample: 配置格式 示例开始...", lm::tips);
	lgc("  ConfigFileName: cfg.ini");
	lgc("  ConfigItem: \n    config1(key1=value1, key2=value2)\n    config2(key3=value3)\n");
	lgc("  cfg.ini: 下面是实际的文本内容");
	lgc("  [config1]");
	lgc("  key1=value1");
	lgc("  key2=value2");
	lgc("  [config2]");
	lgc("  key3=value3");
	lgc("配置格式 示例结束!", lm::tips);
}

void Typical_Tool::StringManage::ConfigFileTextManage::AddConfig_FormatSampleText()
{
	this->OtherCharMap.insert(std::make_pair(0, "//配置格式 示例\n// [ping-baidu]\n\
// 参数=/k ping -t www.baidu.com\n// 文件=cmd\n// 显示窗口=是 (是/否)\n\
// 模式=打开文件 (打开文件/打开文件夹/管理员运行)\n// 菜单按键=是 (是/否: 托盘菜单中添加/程序启动时运行)\n\n"));
}

bool Typical_Tool::StringManage::WriteFile(
	const Tstr& _ConfigFilePath,
	std::vector<Tstr>& _WriteText,
	const Tstr& _FileEncode)
{
	std::ofstream WriteFileStream(_ConfigFilePath, std::ios::out);

	if (!WriteFileStream) {
		lgc("WriteFile: [" + _ConfigFilePath + "] 打开文件失败!", lm::war);
		return false;
	}
	lgc("WriteFile: [" + _ConfigFilePath + "] 打开文件成功!", lm::tips);

	if (_FileEncode == "UTF-8BOM") {
		// 写入 UTF-8 BOM (EF BB BF)
		lgc("WriteFile: 写入 UTF-8BOM编码(EF BB BF).", lm::tips);

		WriteFileStream.put((Tchar)0xEF);
		WriteFileStream.put((Tchar)0xBB);
		WriteFileStream.put((Tchar)0xBF);
	}
	else if (_FileEncode == "UTF-8") {
		lgc("WriteFile: 写入 UTF-8编码.", lm::tips);
	}

	for (const Tstr& tempStr : _WriteText) {
		WriteFileStream << tempStr;
	}
	lgc("WriteFile: 写入完成!", lm::tips);

	return true;
}

bool Typical_Tool::StringManage::ReadFile(
	const Tstr& _ConfigFilePath,
	std::vector<Tstr>& _ReadText,
	const Tstr& _FileEncode)
{
	std::ifstream ReadFileStream(_ConfigFilePath, std::ios::binary);

	if (!ReadFileStream) {
		lgc("ReadFile: [" + _ConfigFilePath + "] 打开文件失败!", lm::war);
		return false;
	}
	lgc("ReadFile: [" + _ConfigFilePath + "] 打开文件成功!", lm::tips);

	// 读取 BOM（如果存在）
	if (_FileEncode == "UTF-8") {
		std::vector<unsigned char> bom(3);
		ReadFileStream.read(reinterpret_cast<char*>(bom.data()), 3);

		// 检查 BOM 是否为 UTF-8 BOM (EF BB BF)
		if (bom.size() == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
			lgc("ReadFile: 检测到UTF-8 BOM.", lm::tips);
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
	lgc("ReadFile: 读取完成!");

	return true;
}