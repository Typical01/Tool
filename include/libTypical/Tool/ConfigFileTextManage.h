#pragma once

//配置文件的字符处理
#ifndef _CONFIGFILETEXTMANAGE_H
#define _CONFIGFILETEXTMANAGE_H

#include <libTypical/Tool/Log.h>
#include <libTypical/Tool/CommonTools.h>


namespace Typical_Tool {
	namespace StringManage {
		//* Encode: 编码
		//写入文件
		template<class T = bool>
		bool WriteFile(const Tstr& _ConfigFilePath, std::vector<Tstr>& _WriteText)
		{
			Tofstream WriteFileStream(_ConfigFilePath, std::ios::out);

			if (!WriteFileStream) {
				log_Namespace(Tx("WriteFile: [") + _ConfigFilePath + Tx("] 打开文件失败!"), War);
				return false;
			}
			log_Namespace(Tx("WriteFile: [") + _ConfigFilePath + Tx("] 打开文件成功!"), Tip);

			for (const Tstr& tempStr : _WriteText) {
				WriteFileStream << tempStr;
			}
			log_Namespace(Tx("WriteFile: 写入完成!"), Tip);

			return true;
		}
		//* Encode: 编码
		//读取文件
		template<class T = bool>
		bool ReadFile(const Tstr& _ConfigFilePath, std::vector<Tstr>& _ReadText)
		{
			Tifstream ReadFileStream(_ConfigFilePath, std::ios::binary);

			if (!ReadFileStream) {
				log_Namespace(Tx("ReadFile: [") + _ConfigFilePath + Tx("] 打开文件失败!"), War);
				return false;
			}
			log_Namespace(Tx("ReadFile: [") + _ConfigFilePath + Tx("] 打开文件成功!"), Tip);

			Tstr line;
			//获取行
			while (std::getline(ReadFileStream, line)) {
				_ReadText.push_back(line); //添加到 _Text
			}
			log_Namespace(Tx("ReadFile: 读取完成!"));

			return true;
		}



		//class ConfigValue {
		//private:
		//	std::map<Tstr, Tstr> Config; //配置
		//	std::shared_ptr<class ConfigFileTextManage> ConfigFileTextManage;
		//	Log& log = lgc;

		//public:
		//	ConfigValue() {}

		//	ConfigValue(std::shared_ptr<class tytl::StringManage::ConfigFileTextManage> _ConfigFileTextManage, std::map<Tstr, Tstr>& _Config)
		//		: ConfigFileTextManage(_ConfigFileTextManage), Config(_Config)
		//	{

		//	}

		//	Tstr operator[](const Tstr& _ConfigItem) {
		//		if (Config.empty()) {
		//			log(Format(Tx("ConfigValue::operator[] Config为空!")));
		//			return Tstr();
		//		}

		//		return ConfigFileTextManage->GetConfigItemValue(Config, _ConfigItem);
		//	}
		//};



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
		class ConfigFileTextManage
		{
		private:
			Tstr ConfigFilePath; //配置文件路径

			std::vector<Tstr> TextCache; //文本缓存

			std::map<Tstr, std::map<Tstr, Tstr>> ConfigMap; //总配置
			std::map<int, Tstr> OtherCharMap; //其他非配置项

			bool ShowManageLog = false; //显示配置处理过程日志
			bool FormatText = false;

			Log& log = lgc;

		public:

			template<class T = bool>
			void README() const
			{
				log(Tx("编码问题:"), Tip);
				log(Tx("Windows: 使用之前先统一编码[Unicode(UTF-8)]: 文件编码, 控制台/其它显示输出程序 编码, 源文件编码"), Tip);
			}

			/* 输出: CMD
			* ShowManageLog: 显示配置处理过程日志
			*/
			ConfigFileTextManage(bool _ShowManageLog = true)
				: ShowManageLog(_ShowManageLog)
			{
			}

			void SetLog(Log& _Log) { log = _Log; }

			~ConfigFileTextManage();

		public:
			/* 初始化
			* _Analyze: 解析文本为 配置格式
			*/
			template<class T = bool>
			bool Init(const Tstr& _ConfigFilePath, bool _Analyze = true) {
				this->ConfigFilePath = _ConfigFilePath; //保存路径

				if (_Analyze) {
					if (!ReadConfigFile()) { //读取配置文件
						log(Tx("Init: 读取配置文件失败!"), War);
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
						log(Tx("Init: 不解析文本!"), Tip);
					}
					return false;
				}

				return true;
			}
			/* 初始化
			* _ReadText: 只读取文本
			*/
			template<class T = bool>
			bool InitText(const Tstr& _ConfigFilePath, std::vector<Tstr>& _ReadText) {
				this->ConfigFilePath = _ConfigFilePath; //保存路径

				if (!ReadConfigFile(_ReadText)) { //读取配置文件
					log(Tx("InitText: 读取配置文件失败!"), War);
					return false;
				}

				return true;
			}

			//设置 显示处理过程的日志
			void SetShowManageLog(bool _IsShowManageLog);

			//获取 配置文件路径
			Tstr GetConfigFilePath() { return this->ConfigFilePath; }

			//获取 其他非配置项
			std::map<int, Tstr> GetOtherCharMap() { return this->OtherCharMap; }

		public:
			//获取 配置项 Value
			Tstr GetConfigItemValue(const Tstr& _Config, const Tstr& _ConfigItem) {
				auto tempConfig = this->ConfigMap.find(_Config);
				if (tempConfig != this->ConfigMap.end()) {
					if (this->ShowManageLog) {
						log(Tx("GetConfigItem: 获取配置"));
						log(tytl::Format(Tx("  配置[%]"), _Config));
					}

					auto tempConfigItem = tempConfig->second.find(_ConfigItem);
					if (tempConfigItem != tempConfig->second.end()) {
						if (this->ShowManageLog) {
							log(tytl::Format(Tx("    配置项 Key  [%]"), _ConfigItem));
							log(tytl::Format(Tx("    配置项 Value[%]"), tempConfigItem->second));
						}

						return tempConfigItem->second;
					}
					else {
						log(Tx("GetConfigItem: 没有找到对应配置项!"), War);
						return Tstr();
					}
				}
				else {
					log(Tx("GetConfigItem: 没有找到对应配置!"), War);
					return Tstr();
				}
			}
			Tstr GetConfigItemValue(std::map<Tstr, Tstr>& _Config, const Tstr& _ConfigItem) {
				auto tempConfigItem = _Config.find(_ConfigItem);
				if (tempConfigItem != _Config.end()) {
					if (this->ShowManageLog) {
						log(tytl::Format(Tx("    配置项 Key  [%]"), _ConfigItem));
						log(tytl::Format(Tx("    配置项 Value[%]"), tempConfigItem->second));
					}

					return tempConfigItem->second;
				}
				else {
					log(Tx("GetConfigItem: 没有找到对应配置项!"), War);
					return Tstr();
				}
			}

			//获取 配置项
			template<class T = bool>
			std::map<Tstr, Tstr> GetConfigItem(const Tstr& _Config) {
				auto tempConfig = this->ConfigMap.find(_Config);
				if (tempConfig != this->ConfigMap.end()) {
					if (this->ShowManageLog) {
						log(Tx("GetConfig: 获取配置"));
						log(tytl::Format(Tx("  配置[%]"), _Config));
					}

					auto& tempConfigItemMap = tempConfig->second;

					//返回
					return std::map<Tstr, Tstr>(tempConfigItemMap.begin(), tempConfigItemMap.end());
				}
				else {
					log(Tx("GetConfig: 没有找到对应配置!"), War);
					return std::map<Tstr, Tstr>();
				}
			}
			//ConfigValue operator[](const Tstr& _Config) {
			//	auto tempConfig = this->ConfigMap.find(_Config);
			//	if (tempConfig != this->ConfigMap.end()) {
			//		if (this->ShowManageLog) {
			//			log(Tx("operator[]: 获取配置"));
			//			log(tytl::Format(Tx("  配置[%]"), _Config));
			//		}

			//		auto& tempConfigItemMap = tempConfig->second;

			//		//返回
			//		return ConfigValue(std::make_shared<ConfigFileTextManage>(this), std::map<Tstr, Tstr>(tempConfigItemMap.begin(), tempConfigItemMap.end()));
			//	}
			//	else {
			//		log(Tx("operator[]: 没有找到对应配置!"), War);
			//		return ConfigValue();
			//	}
			//}

			//获取 全部配置
			template<class T = bool>
			std::map<Tstr, std::map<Tstr, Tstr>> GetConfigMap() {
				return this->ConfigMap;
			}
			//输出 配置文件所有内容
			template<class T = bool>
			void OutConfigFile_All() const
			{
				log(Tx("OutConfigFile_All: 输出内容 ") + this->ConfigFilePath + Tx(" 开始..."), Tip);

				for (auto tempConfig = this->ConfigMap.begin(); tempConfig != this->ConfigMap.end(); tempConfig++) {
					log(Tx("  配置[") + tempConfig->first + Tx("]"));
					for (auto tempConfigText = tempConfig->second.begin(); tempConfigText != tempConfig->second.end(); tempConfigText++) {
						log(Tx("    配置项 Key  : ") + tempConfigText->first);
						log(Tx("    配置项 Value: ") + tempConfigText->second);
					}
				}
				log(Tx("OutConfigFile_All: 输出内容 ") + this->ConfigFilePath + Tx(" 结束!\n"), Tip);
			}

			/* 添加 配置
			* 设置格式: "config"
			* 设置项格式: "key=value"
			*/
			template<class T = bool>
			bool AddConfig(const Tstr& _Config, std::vector<Tstr>& _ConfigItem)
			{
				//先解析为 map
				std::map<Tstr, Tstr> AnalyzeLaterConfigItemMap; //解析后的配置项 Map
				Tstr tempAnalyzeLaterConfigItem_Key; //解析后的配置项 Key
				Tstr tempAnalyzeLaterConfigItem_Value; //解析后的配置项 Value
				for (auto tempConfigItem = _ConfigItem.begin(); tempConfigItem != _ConfigItem.end(); tempConfigItem++) {
					if (!this->ConfigItemTextManage(*tempConfigItem, tempAnalyzeLaterConfigItem_Key, tempAnalyzeLaterConfigItem_Value)) {
						log(Tx("AddConfig: 配置项文本处理失败!"), War);
						log(Tx("  配置[") + _Config + Tx("]"), War);
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
						log(Tx("AddConfig: 重复配置[") + _Config + Tx("]添加配置项开始..."), Tip);
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
						log(Tx("AddConfig: 配置[") + _Config + Tx("]添加配置项开始..."), Tip);
					}
				}

				//临时存放
				std::map<Tstr, Tstr> tempAddConfigItemMap;
				for (auto& ConfigItem : AnalyzeLaterConfigItemMap) {
					tempAddConfigItemMap.insert(std::make_pair(ConfigItem.first, ConfigItem.second));

					if (this->ShowManageLog) {
						log(Tx("    配置项 Key  [") + ConfigItem.first + Tx("]"));
						log(Tx("    配置项 Value[") + ConfigItem.second + Tx("]"));
					}
				}

				//添加到 总配置
				std::pair<std::map<Tstr, std::map<Tstr, Tstr>>::iterator, bool> tempPair \
					= this->ConfigMap.emplace(std::make_pair(_Config, tempAddConfigItemMap));
				if (!tempPair.second) {
					log(Tx("AddConfig: 配置[") + _Config + Tx("]添加失败!"), War);
					return false;
				}
				else {
					log(Tx("AddConfig: 配置[") + _Config + Tx("]添加成功!"), Tip);
				}

				return true;
			}
			template<class T = bool>
			bool AddConfig(const Tstr& _Config, std::map<Tstr, Tstr>& _ConfigItem)
			{
				std::map<Tstr, Tstr> RepeatConfigItemMap; //重复配置Map
				//添加有匹配的配置时, 不添加重复配置项
				auto tempConfig = this->ConfigMap.find(_Config);
				if (tempConfig != this->ConfigMap.end()) { //重复配置
					if (this->ShowManageLog) {
						log(Tx("AddConfig: 重复配置[") + _Config + Tx("]添加配置项开始..."), Tip);
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
						log(Tx("AddConfig: 配置[") + _Config + Tx("]添加配置项开始..."), Tip);
					}
				}

				//临时存放
				std::map<Tstr, Tstr> tempAddConfigItemMap;
				for (auto& ConfigItem : _ConfigItem) {
					tempAddConfigItemMap.insert(std::make_pair(ConfigItem.first, ConfigItem.second));

					if (this->ShowManageLog) {
						log(Tx("  配置项 Key  [") + ConfigItem.first + Tx("]"));
						log(Tx("  配置项 Value[") + ConfigItem.second + Tx("]"));
					}
				}

				//添加到 总配置
				std::pair<std::map<Tstr, std::map<Tstr, Tstr>>::iterator, bool> tempPair \
					= this->ConfigMap.emplace(std::make_pair(_Config, tempAddConfigItemMap));
				if (!tempPair.second) {
					log(Tx("AddConfig: 配置[") + _Config + Tx("] 添加失败!"), War);
					return false;
				}
				else {
					log(Tx("AddConfig: 配置[") + _Config + Tx("] 添加成功!"), Tip);
				}

				return true;
			}

			//删除 配置项: 删除项时为 true, 否则为 false
			template<class T = bool>
			bool DeleteConfigItem(const Tstr& _Config, const Tstr& _ConfigItem)
			{
				//需要删除的配置项是否匹配
				auto temp = this->ConfigMap.find(_Config); //对应配置
				if (temp != this->ConfigMap.end()) {
					if (temp->second.erase(_ConfigItem)) { //删除对应配置项
						if (this->ShowManageLog) {
							log(Tx("DeleteConfigItem: 删除配置项"));
							log(Tx("  配置[") + _Config + Tx("]"));
							log(Tx("    配置项[") + _ConfigItem + Tx("]"));
						}
						return true;
					}
					else {
						log(Tx("DeleteConfigItem: 没有找到对应配置项!"), War);
						return false;
					}
				}
				else {
					log(Tx("DeleteConfigItem: 没有找到对应配置!"), War);
					return false;
				}
			}
			//删除 配置: 删除配置时为 true, 否则为 false; 需要二次确认
			template<class T = bool>
			bool DeleteConfig(const Tstr& _Config, bool _IsDelete)
			{
				if (_IsDelete) {
					if (this->ConfigMap.erase(_Config)) {  //删除对应配置
						if (this->ShowManageLog) {
							log(Tx("DeleteConfig: 删除配置"));
							log(Tx("  配置[") + _Config + Tx("]"));
						}
						return true;
					}
					else {
						log(Tx("DeleteConfig: 没有找到对应配置!"), War);
						return false;
					}
				}
				else {
					log(Tx("DeleteConfig: 未进行二次确认[") + _Config + Tx("]"), War);
					return false;
				}
			}

			//修改 配置项 Value
			template<class T = bool>
			bool SetConfigItemValue(const Tstr& _Config, const Tstr& _ConfigItem_Key, const Tstr& _ConfigItem_Value)
			{
				auto tempConfig = this->ConfigMap.find(_Config); //对应配置
				if (tempConfig != this->ConfigMap.end()) {
					if (this->ShowManageLog) {
						log(Tx("SetConfigItemValue: 修改配置项 Value"));
						log(Tx("  配置[") + _Config + Tx("]"));
					}
					auto tempConfigItem = tempConfig->second.find(_ConfigItem_Key); //对应配置
					if (tempConfigItem != tempConfig->second.end()) {
						if (this->ShowManageLog) {
							log(Tx("    配置项 Key  [") + tempConfigItem->first + Tx("]"));
							log(Tx("    配置项 Value[") + tempConfigItem->second + Tx("]"));
						}

						tempConfig->second.at(_ConfigItem_Key) = _ConfigItem_Value; //修改配置项

						if (this->ShowManageLog) {
							log(Tx("SetConfigItemValue: 修改后..."), Tip);
							log(Tx("    配置项 Value[") + _ConfigItem_Value + Tx("]"));
						}
					}

					return true;
				}
				else {
					log(Tx("SetConfigItemValue: 没有找到对应配置项!"), War);
					return false;
				}
			}
			//修改 配置项 Key
			template<class T = bool>
			bool SetConfigItemKey(const Tstr& _Config, const Tstr& _ConfigItem_Key, const Tstr& _ConfigItem_NewKey)
			{
				auto tempConfig = this->ConfigMap.find(_Config); //对应配置
				if (tempConfig != this->ConfigMap.end()) {
					if (this->ShowManageLog) {
						log(Tx("SetConfigItemKey: 修改配置项 Key"));
						log(Tx("  配置[") + _Config + Tx("]"));
					}
					auto tempConfigItem = tempConfig->second.find(_ConfigItem_Key); //对应配置
					if (tempConfigItem != tempConfig->second.end()) {
						if (this->ShowManageLog) {
							log(Tx("    配置项 Key  [") + tempConfigItem->first + Tx("]"));
							log(Tx("    配置项 Value[") + tempConfigItem->second + Tx("]"));
						}

						auto tempConfigItem_OldValue = tempConfigItem->second;
						//修改配置项
						if (tempConfig->second.erase(_ConfigItem_Key)) {
							tempConfig->second.insert(std::make_pair(_ConfigItem_NewKey, tempConfigItem_OldValue));

							if (this->ShowManageLog) {
								log(Tx("SetConfigItemKey: 修改后..."), Tip);
								log(Tx("    配置项 Key[") + _ConfigItem_NewKey + Tx("]"));
							}
						}
						else {
							log(Tx("SetConfigItemKey: 删除旧配置项 Key 失败! 添加新配置项 Key 操作被跳过!"), War);
							return false;
						}
					}

					return true;
				}
				else {
					log(Tx("SetConfigItemKey: 没有找到对应配置项!"), War);
					return false;
				}
			}
			//修改 配置
			template<class T = bool>
			bool SetConfig(const Tstr& _Config, const Tstr& _NewConfig)
			{
				auto tempConfig = this->ConfigMap.find(_Config); //对应配置
				if (tempConfig != this->ConfigMap.end()) {
					if (this->ShowManageLog) {
						log(Tx("SetConfig: 修改配置"), Tip);
						log(Tx("  配置[") + _Config + Tx("]"), Tip);
					}

					//保存旧配置的配置项
					auto OldConfigMap = tempConfig->second;

					if (this->ConfigMap.erase(_Config)) { //删除旧配置
						this->ConfigMap.insert(std::make_pair(_NewConfig, OldConfigMap)); //添加新配置和保存的旧配置项

						if (this->ShowManageLog) {
							log(Tx("SetConfig: 修改配置[") + _Config + Tx("]成功!"), Tip);
						}
					}
					else {
						log(Tx("SetConfig: 删除旧配置[") + _Config + Tx("]失败!"), War);
						return false;
					}

					return true;
				}
				else {
					log(Tx("SetConfig: 没有找到对应配置项!"), War);
					return false;
				}
			}

			//创建文件
			template<class T = bool>
			bool CreateFile()
			{
				if (!WriteFile(this->ConfigFilePath, this->TextCache)) {
					log(Tx("CreateFile: 创建文件失败!"), War);
					return false;
				}

				log(Tx("CreateFile: 创建文件成功!"), Tip);
				return true;
			}
			template<class T = bool>
			bool CreateFile(const Tstr& _NewFilePath)
			{
				if (!WriteFile(_NewFilePath, this->TextCache)) {
					log(Tx("CreateFile: 创建文件失败!"), War);
					return false;
				}

				log(Tx("CreateFile: 创建文件成功!"), Tip);
				return true;
			}

		public:
			/* 写入文件 : 格式化后的配置
			* _IsSureWrite: 强制写入
			*/
			template<class T = bool>
			bool WriteConfigFile(bool _IsSureWrite = true)
			{
				if (!this->FormatText) {
					if (!_IsSureWrite) {
						std::vector<Tstr> tempWriteConfig_Vec;
						if (this->Format(tempWriteConfig_Vec)) {
							if (!WriteFile(this->ConfigFilePath, tempWriteConfig_Vec)) {
								log(Tx("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!"), War);
								return false;
							}

							if (this->ShowManageLog) {
								if (tempWriteConfig_Vec.size() > 2)
								{
									int tempOtherCharLine_Count = 1;
									log(Tx("WriteConfigFile: 写入过程..."));
									for (auto tempBegin = tempWriteConfig_Vec.begin(); tempBegin != tempWriteConfig_Vec.end(); tempBegin++)
									{
										auto tempOtherChar = this->OtherCharMap.find(tempOtherCharLine_Count);
										if (tempOtherChar != this->OtherCharMap.end()) {
											log(Tx("  写入其他字符: ") + tempOtherChar->second, Lnf);
										}
										log(Tx("  写入字符: ") + *tempBegin, Lnf);

										tempOtherCharLine_Count++;
									}
								}

								log(Tx("WriteConfigFile: 配置写入文件成功!"), Tip);
							}

							this->FormatText = true; //已经格式化
							this->TextCache = tempWriteConfig_Vec;

							return true;
						}
						else {
							log(Tx("WriteConfigFile: 格式化配置文本失败!"), War);
							return false;
						}
					}
					else { //强制写入
						std::vector<Tstr> tempWriteConfig_Vec;
						this->Format(tempWriteConfig_Vec); //无视格式化是否成功
						if (!WriteFile(this->ConfigFilePath, tempWriteConfig_Vec)) {
							log(Tx("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!"), War);
							return false;
						}

						if (this->ShowManageLog) {
							if (tempWriteConfig_Vec.size() > 2)
							{
								int tempOtherCharLine_Count = 1;
								log(Tx("WriteConfigFile: 写入过程..."));
								for (auto tempBegin = tempWriteConfig_Vec.begin(); tempBegin != tempWriteConfig_Vec.end(); tempBegin++)
								{
									auto tempOtherChar = this->OtherCharMap.find(tempOtherCharLine_Count);
									if (tempOtherChar != this->OtherCharMap.end()) {
										log(Tx("  写入其他字符: ") + tempOtherChar->second, Lnf);
									}
									log(Tx("  写入字符: ") + *tempBegin, Lnf);

									tempOtherCharLine_Count++;
								}
							}

							log(Tx("WriteConfigFile: 配置写入文件成功!"), Tip);
						}

						this->FormatText = true; //已经格式化
						this->TextCache = tempWriteConfig_Vec;

						return true;
					}
				}
				else {
					if (this->ShowManageLog) {
						log(Tx("WriteConfigFile: 已格式化配置文本!"), Tip);
					}

					if (!WriteFile(this->ConfigFilePath, this->TextCache)) {
						lgcr(Tx("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!"), War);
						return false;
					}

					return true;
				}
			}
			//写入文件: 文本
			template<class T = bool>
			bool WriteTextFile(std::vector<Tstr>& _Text)
			{
				if (!WriteFile(this->ConfigFilePath, _Text)) {
					log(Tx("WriteTextFile: 可能没有对应的文件, 或文件正在被使用!"), War);
					return false;
				}
				else {
					if (this->ShowManageLog) {
						for (auto tempBegin = _Text.begin(); tempBegin != _Text.end(); tempBegin++)
						{
							if (this->ShowManageLog) {
								log(Tx("  写入字符: ") + *tempBegin);
							}
						}

					}

					log(Tx("WriteTextFile: 文本写入文件成功!"), Tip);
					return true;
				}
			}
			//读取文件: 配置
			template<class T = bool>
			bool ReadConfigFile()
			{
				//读取配置文件
				if (!ReadFile(this->ConfigFilePath, this->TextCache)) {
					log(Tx("ReadConfigFile: 可能没有对应的文件, 或文件正在被使用!"), War);
					return false;
				}

				if (this->ShowManageLog) {
					for (auto tempBegin = this->TextCache.begin(); tempBegin != this->TextCache.end(); tempBegin++)
					{
						log(Tx("  读取字符: ") + *tempBegin, Lnf);

					}

				}

				log(Tx("ReadConfigFile: 文件读取配置成功!"), Tip);
				return true;
			}
			//读取文件: 文本
			template<class T = bool>
			bool ReadConfigFile(std::vector<Tstr>& _Text)
			{//读取配置文件
				if (!ReadFile(this->ConfigFilePath, _Text)) {
					log(Tx("ReadConfigFile: 可能没有对应的文件, 或文件正在被使用!"), War);

					return false;
				}

				if (this->ShowManageLog) {
					for (auto tempBegin = _Text.begin(); tempBegin != _Text.end(); tempBegin++)
					{
						log(Tx("  读取字符: ") + *tempBegin, Lnf);
					}

				}

				log(Tx("ReadConfigFile: 文件读取配置成功!"), Tip);
				return true;
				return false;
			}

		private:
			//将文本格式化为配置
			template<class T = bool>
			bool Format(std::vector<Tstr>& _FormatMap)
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
						log(Tx("Format: 格式化文本输出开始..."), Tip);
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
						auto tempConfigText = Tx("[") + tempConfig->first + Tx("]");
						if (this->ShowManageLog) {
							log(tempConfigText);
						}
						if (FirstLineText) { //第一个配置不需要
							_FormatMap.push_back(tempConfigText + Tx("\n")); //带换行符
							FirstLineText = false;
						}
						else { //每一个配置中间使用换行隔开
							_FormatMap.push_back(Tx("\n"));
							_FormatMap.push_back(tempConfigText + Tx("\n")); //带换行符
						}

						//配置项
						for (auto tempConfigItem = tempConfig->second.begin(); tempConfigItem != tempConfig->second.end(); tempConfigItem++)
						{
							auto tempText = tempConfigItem->first + Tx("=") + tempConfigItem->second;
							if (this->ShowManageLog) {
								log(tempText);
							}
							if (tempConfigItem->second.find('\n') == Tstr::npos)
							{
								_FormatMap.push_back(tempText + Tx("\n")); //带换行符
							}
							else //有换行符
							{
								_FormatMap.push_back(tempText);
							}
						}
					}
					if (this->ShowManageLog) {
						log(Tx("Format: 格式化文本输出结束!"), Tip);
					}

					return true;
				}
				else //空配置
				{
					_FormatMap.push_back(Tx(""));

					log(Tx("Format: 配置项不足以形成格式!"), War);
					ConfigFormatSample();
					return false;
				}
			}
			/*
			* 解析文本内容
			* Analyze: 默认去掉换行符, 即str.size()-1
			* UTF-8 无法 str.find('\n') -> return == str::npos
			*/
			template<class T = bool>
			bool Analyze()
			{
				//配置项非空
				if (!(this->TextCache.size() < 2)) {
					Tstr AnalyzeLaterConfigItem_Key; //解析后配置项 键
					Tstr AnalyzeLaterConfigItem_Value; //解析后配置项 值
					std::map<Tstr, Tstr> AnalyzeLaterConfigItem; //解析后配置项
					Tstr NewConfig; //新的配置: 永远是最新的
					Tstr OldConfig; //旧的配置

					if (this->ShowManageLog) {
						log(Tx("Analyze: 解析内容开始..."), Tip);
					}

					/* 配置/配置项 划分 */
					for (int i = 0; i < this->TextCache.size(); i++) {
						Tstr tempAnalyzeText = this->TextCache[i]; //解析的文本
						if (this->ShowManageLog) {
							log(Tx("解析内容: ") + tempAnalyzeText);
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
									log(Tx("  配置[") + tempAnalyzeText + Tx("]"));
								}
							}
						}
						else if (StrFormat == ConfigStringFormat::ConfigItem) { //配置项
							if (!NewConfig.empty()) { //有配置
								//配置项处理
								if (this->ConfigItemTextManage(tempAnalyzeText, AnalyzeLaterConfigItem_Key, AnalyzeLaterConfigItem_Value)) {
									if (AnalyzeLaterConfigItem_Key.empty()) { //Key为空
										log(Tx("Analyze: 传入的配置项[Key]格式有误!"), War);
										log(Tx("    Key: ") + AnalyzeLaterConfigItem_Key, War);
									}
									if (AnalyzeLaterConfigItem_Value.empty()) { //Value为空
										log(Tx("Analyze: 传入的配置项[Value]格式有误!"), War);
										log(Tx("    Value: ") + AnalyzeLaterConfigItem_Value, War);
									}

									AnalyzeLaterConfigItem.insert(std::make_pair(AnalyzeLaterConfigItem_Key, AnalyzeLaterConfigItem_Value)); //保存到 配置项map
									if (this->ShowManageLog) {
										log(Tx("    配置项 Key  [") + AnalyzeLaterConfigItem_Key + Tx("]"));
										log(Tx("    配置项 Value[") + AnalyzeLaterConfigItem_Value + Tx("]"));
									}
								}
							}
							else {
								//记录 其他字符
								OtherCharMap.insert(std::make_pair(i + 1, tempAnalyzeText));

								if (this->ShowManageLog) {
									log(Tx("* 其他字符串行: ") + ToStr(i));
								}
							}
						}
						else if (StrFormat == ConfigStringFormat::OtherChar) {
							//记录 其他字符
							OtherCharMap.insert(std::make_pair(i + 1, tempAnalyzeText));

							if (this->ShowManageLog) {
								log(Tx("* 其他字符串行: ") + ToStr(i));
							}
						}
					}
					//最后一个配置
					this->ConfigMap.insert(std::make_pair(NewConfig, AnalyzeLaterConfigItem)); //保存到 配置map
					this->TextCache.clear();

					if (this->ShowManageLog) {
						log(Tx("Analyze: 解析完成!"), Tip);
					}

					return true;
				}
				else {
					log(Tx("Analyze: 传入的配置项不足以形成格式!"), War);
					ConfigFormatSample();
					return false;
				}
			}
			/* 解析格式
			* 除最后一行没有换行符以外
			* 其他每行删除 '\n' 的字节数(1)
			*/
			template<class T = bool>
			ConfigStringFormat AnalyzeFormat(Tstr& _AnalyzeConfig)
			{
				if (!_AnalyzeConfig.empty()) {
					//首字符符号
					Tchar tempConfigCharBegin = _AnalyzeConfig[0];
					Tchar tempConfigCharEnd = _AnalyzeConfig[_AnalyzeConfig.size() - 1];

					//字符串是否非法
					if (tempConfigCharBegin == Tx('[') && tempConfigCharEnd == Tx(']')) { //配置
						return ConfigStringFormat::Config;
					}
					else if (tempConfigCharBegin >= 0 && tempConfigCharBegin <= 31) { //控制字符: 0~31
						if (this->ShowManageLog) {
							log(Tx("AnalyzeFormat: OtherChar[") + _AnalyzeConfig + Tx("]"), War);
						}
						return ConfigStringFormat::OtherChar;
					}
					else if (tempConfigCharBegin == 127) { //控制字符
						if (this->ShowManageLog) {
							log(Tx("AnalyzeFormat: OtherChar[") + _AnalyzeConfig + Tx("]"), War);
						}
						return ConfigStringFormat::OtherChar;
					}
					else { //其他字符: 英文/中文/...
						return ConfigStringFormat::ConfigItem;
					}
				}
				else {
					log(Tx("AnalyzeFormat: 传入为空字符串!"), War);
					return ConfigStringFormat::OtherChar;
				}
			}

		private:
			//配置文本处理: 分离字符串中的中括号
			template<class T = bool>
			bool ConfigTextManage(Tstr& _Config)
			{
				//格式是否正确
				size_t tempConfigFormat_Right = _Config.find(Tx(']')); //右括号
				if (tempConfigFormat_Right == Tstr::npos) {
					log(Tx("ConfigTextManage: 格式错误!"), War);
					log(Tx("ConfigTextManage: 配置文本[") + _Config + Tx("]"), War);

					return false;
				}

				//保存配置
				Tstr tempConfig(_Config.begin() + 1, _Config.end() - 2);

				_Config = tempConfig;

				return true;
			}
			//配置项文本处理: 分离字符串中的等号; 默认去掉换行符, 即字符串.size() - 1
			template<class T = bool>
			bool ConfigItemTextManage(Tstr& _ConfigItem, Tstr& _AnalyzeLaterConfigItem_Key, Tstr& _AnalyzeLaterConfigItem_Value)
			{
				//格式是否正确
				size_t tempEqualSign = _ConfigItem.find(Tx('=')); //找到 "=" 号的下标
				if (tempEqualSign == Tstr::npos) {
					log(Tx("ConfigItemTextManage: 格式错误!"), War);
					log(Tx("ConfigItemTextManage: 配置项[") + _ConfigItem + Tx("]"), War);
					return false;
				}

				// Windows 换行处理为: \r\n
				// Unix 换行处理为: \n

				//删除文本中的换行符 '\n'
#ifdef _WINDOWS
				if (*(_ConfigItem.end() - 1) == Tx('\n') || *(_ConfigItem.end() - 1) == Tx('\r')) { //Windows 找到 '\n' || '\r'
					_ConfigItem.erase(_ConfigItem.end() - 1, _ConfigItem.end());
					if (this->ShowManageLog) {
						log(Tx("ConfigItemTextManage: 删除换行"), Tip);
						log(Tx("    Text[") + _ConfigItem + Tx("]"));
					}
				}
#else
				if (*(_ConfigItem.end() - 1) == '\n') { //Unix 找到 '\n'
					_ConfigItem.erase(_ConfigItem.end() - 1, _ConfigItem.end());
					if (this->ShowManageLog) {
						log(Tx("ConfigItemTextManage: 删除换行"), Tip);
						log(Tx("    Text[") + _ConfigItem + Tx("]"));
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

		public:
			//配置格式示例
			template<class T = bool>
			void ConfigFormatSample()
			{
				log(Tx("ConfigFormatSample: 配置格式 示例开始..."), Tip);
				log(Tx("  ConfigFileName: cfg.ini"));
				log(Tx("  ConfigItem: \n    config1(key1=value1, key2=value2)\n    config2(key3=value3)\n"));
				log(Tx("  cfg.ini: 下面是实际的文本内容"));
				log(Tx("  [config1]"));
				log(Tx("  key1=value1"));
				log(Tx("  key2=value2"));
				log(Tx("  [config2]"));
				log(Tx("  key3=value3"));
				log(Tx("配置格式 示例结束!"), Tip);
			}
			template<class T = bool>
			void AddConfig_FormatSampleText()
			{
				this->OtherCharMap.insert(std::make_pair(0, Tx("//配置格式 示例\n// [ping-baidu]\n\
// 参数=/k ping -t www.baidu.com\n// 文件=cmd\n// 显示窗口=是 (是/否)\n\
// 模式=打开文件 (打开文件/打开文件夹/管理员运行)\n// 菜单按键=是 (是/否: 托盘菜单中添加/程序启动时运行)\n\n")));
			}

		};

}
}
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