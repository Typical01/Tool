#pragma once

//配置文件的字符处理
#ifndef _CONFIGFILETEXTMANAGE_H
#define _CONFIGFILETEXTMANAGE_H

#include <libTypical/Tool/Log.h>
#include <libTypical/Tool/CommonTools.h>


namespace Typical_Tool {
	namespace StringManage
	{
		//* Encode: 编码
		//写入文件
		template<class T = bool>
		bool WriteFile(const Tstr& _ConfigFilePath, std::vector<Tstr>& _WriteText, const Tstr& _FileEncode = Tx("UTF-8"))
		{
			Tofstream WriteFileStream(_ConfigFilePath, std::ios::out);

			if (!WriteFileStream) {
				lgc(Tx("WriteFile: [") + _ConfigFilePath + Tx("] 打开文件失败!"), wr);
				return false;
			}
			lgc(Tx("WriteFile: [") + _ConfigFilePath + Tx("] 打开文件成功!"), ts);

			if (_FileEncode == Tx("UTF-8BOM")) {
				// 写入 UTF-8 BOM (EF BB BF)
				lgc(Tx("WriteFile: 写入 UTF-8BOM编码(EF BB BF)."), ts);

				WriteFileStream.put((Tchar)0xEF);
				WriteFileStream.put((Tchar)0xBB);
				WriteFileStream.put((Tchar)0xBF);
			}
			else if (_FileEncode == Tx("UTF-8")) {
				lgc(Tx("WriteFile: 写入 UTF-8编码."), ts);
			}

			for (const Tstr& tempStr : _WriteText) {
				WriteFileStream << tempStr;
			}
			lgc(Tx("WriteFile: 写入完成!"), ts);

			return true;
		}
		//* Encode: 编码
		//读取文件
		template<class T = bool>
		bool ReadFile(const Tstr& _ConfigFilePath, std::vector<Tstr>& _ReadText, const Tstr& _FileEncode = Tx("UTF-8"))
		{
			Tifstream ReadFileStream(_ConfigFilePath, std::ios::binary);

			if (!ReadFileStream) {
				lgc(Tx("ReadFile: [") + _ConfigFilePath + Tx("] 打开文件失败!"), wr);
				return false;
			}
			lgc(Tx("ReadFile: [") + _ConfigFilePath + Tx("] 打开文件成功!"), ts);

			// 读取 BOM（如果存在）
			if (_FileEncode == Tx("UTF-8")) {
				std::vector<Tchar> bom(3);
				ReadFileStream.read(reinterpret_cast<Tchar*>(bom.data()), 3);

				// 检查 BOM 是否为 UTF-8 BOM (EF BB BF)
				if (bom.size() == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
					lgc(Tx("ReadFile: 检测到UTF-8 BOM."), ts);
				}
				else {
					// 如果文件没有 BOM，返回文件流的读位置
					ReadFileStream.clear(); // 清除 EOF 标志
					ReadFileStream.seekg(0);
				}
			}

			Tstr line;
			//获取行
			while (std::getline(ReadFileStream, line)) {
				_ReadText.push_back(line); //添加到 _Text
			}
			lgc(Tx("ReadFile: 读取完成!"));

			return true;
		}


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

		public:

			template<class T = bool>
			void README() const
			{
				lgc(Tx("编码问题:"), ts);
				lgc(Tx("Windows: 使用之前先统一编码[Unicode(UTF-8)]: 文件编码, 控制台/其它显示输出程序 编码, 源文件编码"), ts);
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
			template<class T = bool>
			bool Init(const Tstr& _ConfigFilePath, bool _Analyze = true)
			{
				this->ConfigFilePath = _ConfigFilePath; //保存路径

				if (_Analyze) {
					if (!ReadConfigFile()) { //读取配置文件
						lgc(Tx("Init: 读取配置文件失败!"), wr);
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
						lgc(Tx("Init: 不解析文本!"), ts);
					}
					return false;
				}

				return true;
			}
			/* 初始化
			* _ReadText: 只读取文本
			*/
			template<class T = bool>
			bool InitText(const Tstr& _ConfigFilePath, std::vector<Tstr>& _ReadText)
			{
				this->ConfigFilePath = _ConfigFilePath; //保存路径

				if (!ReadConfigFile(_ReadText)) { //读取配置文件
					lgc(Tx("InitText: 读取配置文件失败!"), wr);
					return false;
				}

				return true;
			}

			//设置 显示处理过程的日志
			void SetShowManageLog(bool _IsShowManageLog);

		public:
			//获取 配置项 Value
			template<class T = bool>
			Tstr GetConfigItem_Value(const Tstr& _Config, const Tstr& _ConfigItem)
			{
				auto tempConfig = this->ConfigMap.find(_Config);
				if (tempConfig != this->ConfigMap.end()) {
					if (this->ShowManageLog) {
						lgc(Tx("GetConfigItem_Value: 获取配置项 Value"));
						lgc(Tx("  配置[") + _Config + Tx("]"));
					}

					auto tempConfigItem = tempConfig->second.find(_ConfigItem);
					if (tempConfigItem != tempConfig->second.end()) {
						if (this->ShowManageLog) {
							lgc(Tx("    配置项 Key  [") + _ConfigItem + Tx("]"));
							lgc(Tx("    配置项 Value[") + tempConfigItem->second + Tx("]"));
						}
						return tempConfigItem->second;
					}
					else {
						lgc(Tx("GetConfigItem_Value: 没有找到对应配置项!"), wr);
						return Tstr();
					}
				}
				else {
					lgc(Tx("GetConfigItem_Value: 没有找到对应配置项!"), wr);
					return Tstr();
				}
			}
			//获取 配置项 Key-Value
			template<class T = bool>
			std::map<Tstr, Tstr> GetConfigItem(const Tstr& _Config, const Tstr& _ConfigItem)
			{
				auto tempConfig = this->ConfigMap.find(_Config);
				if (tempConfig != this->ConfigMap.end()) {
					if (this->ShowManageLog) {
						lgc(Tx("GetConfigItem: 获取配置项"));
						lgc(Tx("  配置[") + _Config + Tx("]"));
					}

					auto tempConfigItem = tempConfig->second.find(_ConfigItem);
					if (tempConfigItem != tempConfig->second.end()) {
						if (this->ShowManageLog) {
							lgc(Tx("    配置项[") + _ConfigItem + Tx("]"));
						}

						return { {tempConfigItem->first, tempConfigItem->second} };
					}
					else {
						lgc(Tx("GetConfigItem: 没有找到对应配置项!"), wr);
						return std::map<Tstr, Tstr>();
					}
				}
				else {
					lgc(Tx("GetConfigItem: 没有找到对应配置项!"), wr);
					return std::map<Tstr, Tstr>();
				}
			}
			//获取 配置
			template<class T = bool>
			std::map<Tstr, Tstr> GetConfig(const Tstr& _Config)
			{
				auto tempConfig = this->ConfigMap.find(_Config);
				if (tempConfig != this->ConfigMap.end()) {
					if (this->ShowManageLog) {
						lgc(Tx("GetConfig: 获取配置"));
						lgc(Tx("  配置[") + _Config + Tx("]"));
					}

					auto tempConfigItemMap = tempConfig->second;

					//返回
					return std::map<Tstr, Tstr>(tempConfigItemMap.begin(), tempConfigItemMap.end());
				}
				else {
					lgc(Tx("GetConfig: 没有找到对应配置!"), wr);
					return std::map<Tstr, Tstr>();
				}
			}
			//获取 全部配置
			template<class T = bool>
			std::map<Tstr, std::map<Tstr, Tstr>> GetConfigMap()
			{
				return this->ConfigMap;
			}
			//输出 配置文件所有内容
			template<class T = bool>
			void OutConfigFile_All() const
			{
				lgc(Tx("OutConfigFile_All: 输出内容 ") + this->ConfigFilePath + Tx(" 开始..."), ts);

				for (auto tempConfig = this->ConfigMap.begin(); tempConfig != this->ConfigMap.end(); tempConfig++) {
					lgc(Tx("  配置[") + tempConfig->first + Tx("]"));
					for (auto tempConfigText = tempConfig->second.begin(); tempConfigText != tempConfig->second.end(); tempConfigText++) {
						lgc(Tx("    配置项 Key  : ") + tempConfigText->first);
						lgc(Tx("    配置项 Value: ") + tempConfigText->second);
					}
				}
				lgc(Tx("OutConfigFile_All: 输出内容 ") + this->ConfigFilePath + Tx(" 结束!\n"), ts);
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
						lgc(Tx("AddConfig: 配置项文本处理失败!"), wr);
						lgc(Tx("  配置[") + _Config + Tx("]"), wr);
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
						lgc(Tx("AddConfig: 重复配置[") + _Config + Tx("]添加配置项开始..."), ts);
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
						lgc(Tx("AddConfig: 配置[") + _Config + Tx("]添加配置项开始..."), ts);
					}
				}

				//临时存放
				std::map<Tstr, Tstr> tempAddConfigItemMap;
				for (auto& ConfigItem : AnalyzeLaterConfigItemMap) {
					tempAddConfigItemMap.insert(std::make_pair(ConfigItem.first, ConfigItem.second));

					if (this->ShowManageLog) {
						lgc(Tx("    配置项 Key  [") + ConfigItem.first + Tx("]"));
						lgc(Tx("    配置项 Value[") + ConfigItem.second + Tx("]"));
					}
				}

				//添加到 总配置
				std::pair<std::map<Tstr, std::map<Tstr, Tstr>>::iterator, bool> tempPair \
					= this->ConfigMap.emplace(std::make_pair(_Config, tempAddConfigItemMap));
				if (!tempPair.second) {
					lgc(Tx("AddConfig: 配置[") + _Config + Tx("]添加失败!"), wr);
					return false;
				}
				else {
					lgc(Tx("AddConfig: 配置[") + _Config + Tx("]添加成功!"), ts);
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
						lgc(Tx("AddConfig: 重复配置[") + _Config + Tx("]添加配置项开始..."), ts);
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
						lgc(Tx("AddConfig: 配置[") + _Config + Tx("]添加配置项开始..."), ts);
					}
				}

				//临时存放
				std::map<Tstr, Tstr> tempAddConfigItemMap;
				for (auto& ConfigItem : _ConfigItem) {
					tempAddConfigItemMap.insert(std::make_pair(ConfigItem.first, ConfigItem.second));

					if (this->ShowManageLog) {
						lgc(Tx("  配置项 Key  [") + ConfigItem.first + Tx("]"));
						lgc(Tx("  配置项 Value[") + ConfigItem.second + Tx("]"));
					}
				}

				//添加到 总配置
				std::pair<std::map<Tstr, std::map<Tstr, Tstr>>::iterator, bool> tempPair \
					= this->ConfigMap.emplace(std::make_pair(_Config, tempAddConfigItemMap));
				if (!tempPair.second) {
					lgc(Tx("AddConfig: 配置[") + _Config + Tx("] 添加失败!"), wr);
					return false;
				}
				else {
					lgc(Tx("AddConfig: 配置[") + _Config + Tx("] 添加成功!"), ts);
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
							lgc(Tx("DeleteConfigItem: 删除配置项"));
							lgc(Tx("  配置[") + _Config + Tx("]"));
							lgc(Tx("    配置项[") + _ConfigItem + Tx("]"));
						}
						return true;
					}
					else {
						lgc(Tx("DeleteConfigItem: 没有找到对应配置项!"), wr);
						return false;
					}
				}
				else {
					lgc(Tx("DeleteConfigItem: 没有找到对应配置!"), wr);
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
							lgc(Tx("DeleteConfig: 删除配置"));
							lgc(Tx("  配置[") + _Config + Tx("]"));
						}
						return true;
					}
					else {
						lgc(Tx("DeleteConfig: 没有找到对应配置!"), wr);
						return false;
					}
				}
				else {
					lgc(Tx("DeleteConfig: 未进行二次确认[") + _Config + Tx("]"), wr);
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
						lgc(Tx("SetConfigItemValue: 修改配置项 Value"));
						lgc(Tx("  配置[") + _Config + Tx("]"));
					}
					auto tempConfigItem = tempConfig->second.find(_ConfigItem_Key); //对应配置
					if (tempConfigItem != tempConfig->second.end()) {
						if (this->ShowManageLog) {
							lgc(Tx("    配置项 Key  [") + tempConfigItem->first + Tx("]"));
							lgc(Tx("    配置项 Value[") + tempConfigItem->second + Tx("]"));
						}

						tempConfig->second.at(_ConfigItem_Key) = _ConfigItem_Value; //修改配置项

						if (this->ShowManageLog) {
							lgc(Tx("SetConfigItemValue: 修改后..."), ts);
							lgc(Tx("    配置项 Value[") + _ConfigItem_Value + Tx("]"));
						}
					}

					return true;
				}
				else {
					lgc(Tx("SetConfigItemValue: 没有找到对应配置项!"), wr);
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
						lgc(Tx("SetConfigItemKey: 修改配置项 Key"));
						lgc(Tx("  配置[") + _Config + Tx("]"));
					}
					auto tempConfigItem = tempConfig->second.find(_ConfigItem_Key); //对应配置
					if (tempConfigItem != tempConfig->second.end()) {
						if (this->ShowManageLog) {
							lgc(Tx("    配置项 Key  [") + tempConfigItem->first + Tx("]"));
							lgc(Tx("    配置项 Value[") + tempConfigItem->second + Tx("]"));
						}

						auto tempConfigItem_OldValue = tempConfigItem->second;
						//修改配置项
						if (tempConfig->second.erase(_ConfigItem_Key)) {
							tempConfig->second.insert(std::make_pair(_ConfigItem_NewKey, tempConfigItem_OldValue));

							if (this->ShowManageLog) {
								lgc(Tx("SetConfigItemKey: 修改后..."), ts);
								lgc(Tx("    配置项 Key[") + _ConfigItem_NewKey + Tx("]"));
							}
						}
						else {
							lgc(Tx("SetConfigItemKey: 删除旧配置项 Key 失败! 添加新配置项 Key 操作被跳过!"), wr);
							return false;
						}
					}

					return true;
				}
				else {
					lgc(Tx("SetConfigItemKey: 没有找到对应配置项!"), wr);
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
						lgc(Tx("SetConfig: 修改配置"), ts);
						lgc(Tx("  配置[") + _Config + Tx("]"), ts);
					}

					//保存旧配置的配置项
					auto OldConfigMap = tempConfig->second;

					if (this->ConfigMap.erase(_Config)) { //删除旧配置
						this->ConfigMap.insert(std::make_pair(_NewConfig, OldConfigMap)); //添加新配置和保存的旧配置项

						if (this->ShowManageLog) {
							lgc(Tx("SetConfig: 修改配置[") + _Config + Tx("]成功!"), ts);
						}
					}
					else {
						lgc(Tx("SetConfig: 删除旧配置[") + _Config + Tx("]失败!"), wr);
						return false;
					}

					return true;
				}
				else {
					lgc(Tx("SetConfig: 没有找到对应配置项!"), wr);
					return false;
				}
			}

			//创建文件
			template<class T = bool>
			bool CreateFile(const Tstr& _FileEncode = Tx("UTF-8"))
			{
				if (!WriteFile(this->ConfigFilePath, this->TextCache, _FileEncode)) {
					lgc(Tx("CreateFile: 创建文件失败!"), wr);
					return false;
				}

				lgc(Tx("CreateFile: 创建文件成功!"), ts);
				return true;
			}
			template<class T = bool>
			bool CreateFile(const Tstr& _NewFilePath, const Tstr& _FileEncode = Tx("UTF-8"))
			{
				if (!WriteFile(_NewFilePath, this->TextCache, _FileEncode)) {
					lgc(Tx("CreateFile: 创建文件失败!"), wr);
					return false;
				}

				lgc(Tx("CreateFile: 创建文件成功!"), ts);
				return true;
			}

		public:
			/* 写入文件 : 格式化后的配置
			* _IsSureWrite: 强制写入
			*/
			template<class T = bool>
			bool WriteConfigFile(bool _IsSureWrite = true, const Tstr& _FileEncode = Tx("UTF-8"))
			{
				if (!this->FormatText) {
					if (!_IsSureWrite) {
						std::vector<Tstr> tempWriteConfig_Vec;
						if (this->Format(tempWriteConfig_Vec)) {
							if (!WriteFile(this->ConfigFilePath, tempWriteConfig_Vec, _FileEncode)) {
								lgc(Tx("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!"), wr);
								return false;
							}

							if (this->ShowManageLog) {
								if (tempWriteConfig_Vec.size() > 2)
								{
									int tempOtherCharLine_Count = 1;
									lgc(Tx("WriteConfigFile: 写入过程..."));
									for (auto tempBegin = tempWriteConfig_Vec.begin(); tempBegin != tempWriteConfig_Vec.end(); tempBegin++)
									{
										auto tempOtherChar = this->OtherCharMap.find(tempOtherCharLine_Count);
										if (tempOtherChar != this->OtherCharMap.end()) {
											lgc(Tx("  写入其他字符: ") + tempOtherChar->second, lf);
										}
										lgc(Tx("  写入字符: ") + *tempBegin, lf);

										tempOtherCharLine_Count++;
									}
								}

								lgc(Tx("WriteConfigFile: 配置写入文件成功!"), ts);
							}

							this->FormatText = true; //已经格式化
							this->TextCache = tempWriteConfig_Vec;

							return true;
						}
						else {
							lgc(Tx("WriteConfigFile: 格式化配置文本失败!"), wr);
							return false;
						}
					}
					else { //强制写入
						std::vector<Tstr> tempWriteConfig_Vec;
						this->Format(tempWriteConfig_Vec); //无视格式化是否成功
						if (!WriteFile(this->ConfigFilePath, tempWriteConfig_Vec, _FileEncode)) {
							lgc(Tx("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!"), wr);
							return false;
						}

						if (this->ShowManageLog) {
							if (tempWriteConfig_Vec.size() > 2)
							{
								int tempOtherCharLine_Count = 1;
								lgc(Tx("WriteConfigFile: 写入过程..."));
								for (auto tempBegin = tempWriteConfig_Vec.begin(); tempBegin != tempWriteConfig_Vec.end(); tempBegin++)
								{
									auto tempOtherChar = this->OtherCharMap.find(tempOtherCharLine_Count);
									if (tempOtherChar != this->OtherCharMap.end()) {
										lgc(Tx("  写入其他字符: ") + tempOtherChar->second, lf);
									}
									lgc(Tx("  写入字符: ") + *tempBegin, lf);

									tempOtherCharLine_Count++;
								}
							}

							lgc(Tx("WriteConfigFile: 配置写入文件成功!"), ts);
						}

						this->FormatText = true; //已经格式化
						this->TextCache = tempWriteConfig_Vec;

						return true;
					}
				}
				else {
					if (this->ShowManageLog) {
						lgc(Tx("WriteConfigFile: 已格式化配置文本!"), ts);
					}

					if (!WriteFile(this->ConfigFilePath, this->TextCache, _FileEncode)) {
						lgcr(Tx("WriteConfigFile: 可能没有对应的文件, 或文件正在被使用!"), wr);
						return false;
					}

					return true;
				}
			}
			//写入文件: 文本
			template<class T = bool>
			bool WriteTextFile(std::vector<Tstr>& _Text, const Tstr& _FileEncode = Tx("UTF-8"))
			{
				if (!WriteFile(this->ConfigFilePath, _Text, _FileEncode)) {
					lgc(Tx("WriteTextFile: 可能没有对应的文件, 或文件正在被使用!"), wr);
					return false;
				}
				else {
					if (this->ShowManageLog) {
						for (auto tempBegin = _Text.begin(); tempBegin != _Text.end(); tempBegin++)
						{
							if (this->ShowManageLog) {
								lgc(Tx("  写入字符: ") + *tempBegin);
							}
						}

					}

					lgc(Tx("WriteTextFile: 文本写入文件成功!"), ts);
					return true;
				}
			}
			//读取文件: 配置
			template<class T = bool>
			bool ReadConfigFile(const Tstr& _FileEncode = Tx("UTF-8"))
			{
				//读取配置文件
				if (!ReadFile(this->ConfigFilePath, this->TextCache, _FileEncode)) {
					lgc(Tx("ReadConfigFile: 可能没有对应的文件, 或文件正在被使用!"), wr);
					return false;
				}

				if (this->ShowManageLog) {
					for (auto tempBegin = this->TextCache.begin(); tempBegin != this->TextCache.end(); tempBegin++)
					{
						lgc(Tx("  读取字符: ") + *tempBegin, lf);

					}

				}

				lgc(Tx("ReadConfigFile: 文件读取配置成功!"), ts);
				return true;
			}
			//读取文件: 文本
			template<class T = bool>
			bool ReadConfigFile(std::vector<Tstr>& _Text, const Tstr& _FileEncode = Tx("UTF-8"))
			{//读取配置文件
				if (!ReadFile(this->ConfigFilePath, _Text, _FileEncode)) {
					lgc(Tx("ReadConfigFile: 可能没有对应的文件, 或文件正在被使用!"), wr);

					return false;
				}

				if (this->ShowManageLog) {
					for (auto tempBegin = _Text.begin(); tempBegin != _Text.end(); tempBegin++)
					{
						lgc(Tx("  读取字符: ") + *tempBegin, lf);
					}

				}

				lgc(Tx("ReadConfigFile: 文件读取配置成功!"), ts);
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
						lgc(Tx("Format: 格式化文本输出开始..."), ts);
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
							lgc(tempConfigText);
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
								lgc(tempText);
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
						lgc(Tx("Format: 格式化文本输出结束!"), ts);
					}

					return true;
				}
				else //空配置
				{
					_FormatMap.push_back(Tx(""));

					lgc(Tx("Format: 配置项不足以形成格式!"), wr);
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
						lgc(Tx("Analyze: 解析内容开始..."), ts);
					}

					/* 配置/配置项 划分 */
					for (int i = 0; i < this->TextCache.size(); i++) {
						Tstr tempAnalyzeText = this->TextCache[i]; //解析的文本
						if (this->ShowManageLog) {
							lgc(Tx("解析内容: ") + tempAnalyzeText);
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
									lgc(Tx("  配置[") + tempAnalyzeText + Tx("]"));
								}
							}
						}
						else if (StrFormat == ConfigStringFormat::ConfigItem) { //配置项
							if (!NewConfig.empty()) { //有配置
								//配置项处理
								if (this->ConfigItemTextManage(tempAnalyzeText, AnalyzeLaterConfigItem_Key, AnalyzeLaterConfigItem_Value)) {
									if (AnalyzeLaterConfigItem_Key.empty()) { //Key为空
										lgc(Tx("Analyze: 传入的配置项[Key]格式有误!"), wr);
										lgc(Tx("    Key: ") + AnalyzeLaterConfigItem_Key, wr);
									}
									if (AnalyzeLaterConfigItem_Value.empty()) { //Value为空
										lgc(Tx("Analyze: 传入的配置项[Value]格式有误!"), wr);
										lgc(Tx("    Value: ") + AnalyzeLaterConfigItem_Value, wr);
									}

									AnalyzeLaterConfigItem.insert(std::make_pair(AnalyzeLaterConfigItem_Key, AnalyzeLaterConfigItem_Value)); //保存到 配置项map
									if (this->ShowManageLog) {
										lgc(Tx("    配置项 Key  [") + AnalyzeLaterConfigItem_Key + Tx("]"));
										lgc(Tx("    配置项 Value[") + AnalyzeLaterConfigItem_Value + Tx("]"));
									}
								}
							}
							else {
								//记录 其他字符
								OtherCharMap.insert(std::make_pair(i + 1, tempAnalyzeText));

								if (this->ShowManageLog) {
									lgc(Tx("* 其他字符串行: ") + ToStr(i));
								}
							}
						}
						else if (StrFormat == ConfigStringFormat::OtherChar) {
							//记录 其他字符
							OtherCharMap.insert(std::make_pair(i + 1, tempAnalyzeText));

							if (this->ShowManageLog) {
								lgc(Tx("* 其他字符串行: ") + ToStr(i));
							}
						}
					}
					//最后一个配置
					this->ConfigMap.insert(std::make_pair(NewConfig, AnalyzeLaterConfigItem)); //保存到 配置map
					this->TextCache.clear();

					if (this->ShowManageLog) {
						lgc(Tx("Analyze: 解析完成!"), ts);
					}

					return true;
				}
				else {
					lgc(Tx("Analyze: 传入的配置项不足以形成格式!"), wr);
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
					Tchar tempConfigChar = _AnalyzeConfig[0];

					//字符串是否非法
					if (tempConfigChar == Tx('[')) { //配置
						return ConfigStringFormat::Config;
					}
					else if (tempConfigChar >= 0 && tempConfigChar <= 31) { //控制字符: 0~31
						if (this->ShowManageLog) {
							lgc(Tx("AnalyzeFormat: OtherChar[") + _AnalyzeConfig + Tx("]"), wr);
						}
						return ConfigStringFormat::OtherChar;
					}
					else if (tempConfigChar == 127) { //控制字符
						if (this->ShowManageLog) {
							lgc(Tx("AnalyzeFormat: OtherChar[") + _AnalyzeConfig + Tx("]"), wr);
						}
						return ConfigStringFormat::OtherChar;
					}
					else { //其他字符: 英文/中文/...
						return ConfigStringFormat::ConfigItem;
					}
				}
				else {
					lgc(Tx("AnalyzeFormat: 传入为空字符串!"), wr);
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
					lgc(Tx("ConfigTextManage: 格式错误!"), wr);
					lgc(Tx("ConfigTextManage: 配置文本[") + _Config + Tx("]"), wr);

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
					lgc(Tx("ConfigItemTextManage: 格式错误!"), wr);
					lgc(Tx("ConfigItemTextManage: 配置项[") + _ConfigItem + Tx("]"), wr);
					return false;
				}

				// Windows 换行处理为: \r\n
				// Unix 换行处理为: \n

				//删除文本中的换行符 '\n'
#ifdef _WINDOWS
				if (*(_ConfigItem.end() - 1) == Tx('\n') || *(_ConfigItem.end() - 1) == Tx('\r')) { //Windows 找到 '\n' || '\r'
					_ConfigItem.erase(_ConfigItem.end() - 1, _ConfigItem.end());
					if (this->ShowManageLog) {
						lgc(Tx("ConfigItemTextManage: 删除换行"), ts);
						lgc(Tx("    Text[") + _ConfigItem + Tx("]"));
					}
				}
#else
				if (*(_ConfigItem.end() - 1) == '\n') { //Unix 找到 '\n'
					_ConfigItem.erase(_ConfigItem.end() - 1, _ConfigItem.end());
					if (this->ShowManageLog) {
						lgc(Tx("ConfigItemTextManage: 删除换行"), ts);
						lgc(Tx("    Text[") + _ConfigItem + Tx("]"));
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
				lgc(Tx("ConfigFormatSample: 配置格式 示例开始..."), ts);
				lgc(Tx("  ConfigFileName: cfg.ini"));
				lgc(Tx("  ConfigItem: \n    config1(key1=value1, key2=value2)\n    config2(key3=value3)\n"));
				lgc(Tx("  cfg.ini: 下面是实际的文本内容"));
				lgc(Tx("  [config1]"));
				lgc(Tx("  key1=value1"));
				lgc(Tx("  key2=value2"));
				lgc(Tx("  [config2]"));
				lgc(Tx("  key3=value3"));
				lgc(Tx("配置格式 示例结束!"), ts);
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