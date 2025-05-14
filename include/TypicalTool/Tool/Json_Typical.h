#ifndef _JSON_TYPICAL_H
#define _JSON_TYPICAL_H

#include <TypicalTool/Tool/pch.h>
#include <Json/json.h>
#include <TypicalTool/Tool/Log.h>

namespace Typical_Tool {

	class TYPICALTOOL_API JsonManage {
	private:
		Json::Value Value;
		Json::StreamWriterBuilder WriterBuilder;
		Json::CharReaderBuilder ReaderBuilder;
		std::string JsonFilePath;

	public:
		JsonManage()
		{}
		//Json写入流配置: utf-8非英文字符显示 | 缩进 | 保存文件路径 | 读取并解析Json文件(return bool)
		JsonManage(const std::string& _JsonFilePath, bool _IsReadJsonFile = false)
		{
			this->Init(_JsonFilePath, _IsReadJsonFile);
		}

		//Json写入流配置: utf-8非英文字符显示 | 缩进 | 保存文件路径 | 读取并解析Json文件(return bool)
		bool Init(const std::string& _JsonFilePath, bool _IsReadJsonFile)
		{
			this->WriterBuilder["emitUTF8"] = true; //utf-8字符显示为非 /uxxx
			this->WriterBuilder["indentation"] = "    "; // 设置缩进
			this->JsonFilePath = _JsonFilePath; //保存 Json文件路径

			if (_IsReadJsonFile) {
				return ReadJsonFile(_JsonFilePath);
			}
			return true;
		}
		bool Init(const std::string& _JsonFilePath, bool _IsReadJsonFile, int32_t& _ErrorTips)
		{
			this->WriterBuilder["emitUTF8"] = true; //utf-8字符显示为非 /uxxx
			this->WriterBuilder["indentation"] = "    "; // 设置缩进
			this->JsonFilePath = _JsonFilePath; //保存 Json文件路径

			if (_IsReadJsonFile) {
				return ReadJsonFile(_JsonFilePath, _ErrorTips);
			}
			return true;
		}

	private:
		bool WriteStream(const std::string& _JsonFilePath, Json::Value& _Value, std::ios::ios_base::openmode _StreamOpenMode)
		{
			std::ofstream JsonFileOut(_JsonFilePath, _StreamOpenMode);
			if (!JsonFileOut.is_open()) {
				LogDebug(Printf(TEXT("JsonManage::WriteStream: 打开Json文件失败: !ofstream.is_open()")), Err);
				LogDebug(Printf(TEXT("JsonManage::WriteStream: Json文件路径: [%s]"), stow(_JsonFilePath)), Err);
				return false;
			}
			JsonFileOut << Json::writeString(this->WriterBuilder, _Value);
			return true;
		}

		bool ReadStream(const std::string& _JsonFilePath, Json::Value& _Value)
		{
			std::ifstream JsonFile(_JsonFilePath, std::ifstream::binary);
			std::string ErrorCode;
			if (!JsonFile.is_open()) {
				LogDebug(Printf(TEXT("JsonManage::WriteStream: 打开Json文件失败: [%s]"), stow(ErrorCode)), Err);
				LogDebug(Printf(TEXT("JsonManage::WriteStream: Json文件路径: [%s]"), stow(_JsonFilePath)), Err);
				return false;
			}

			if (!Json::parseFromStream(this->ReaderBuilder, JsonFile, &_Value, &ErrorCode)) {
				LogDebug(Printf(TEXT("JsonManage::WriteStream: 解析 Json失败: [%s]"), stow(ErrorCode)), Err);
				LogDebug(Printf(TEXT("JsonManage::WriteStream: Json文件路径: [%s]"), stow(_JsonFilePath)), Err);
				return false;
			}
			return true;
		}

		bool ReadStream(const std::string& _JsonFilePath, Json::Value& _Value, int32_t& _ErrorTips)
		{
			std::ifstream JsonFile(_JsonFilePath, std::ifstream::binary);

			std::string ErrorCode;
			if (!JsonFile.is_open()) {
				LogDebug(Printf(TEXT("JsonManage::ReadStream: 打开Json文件失败: [%s]"), stow(ErrorCode)), Err);
				LogDebug(Printf(TEXT("JsonManage::ReadStream: Json文件路径: [%s]"), stow(_JsonFilePath)), Err);
				_ErrorTips = -1;

				return false;
			}

			if (!Json::parseFromStream(this->ReaderBuilder, JsonFile, &_Value, &ErrorCode)) {
				LogDebug(Printf(TEXT("JsonManage::ReadStream: 解析 Json失败: [%s]"), stow(ErrorCode)), Err);
				LogDebug(Printf(TEXT("JsonManage::ReadStream: Json文件路径: [%s]"), stow(_JsonFilePath)), Err);
				_ErrorTips = -1;

				return false;
			}
			_ErrorTips = 1;
			return true;
		}

	public:
		//读取 Json文件到 _Value
		bool ReadJsonFile()
		{
			return ReadStream(this->JsonFilePath, this->Value);
		}
		//读取 Json文件到 _Value
		bool ReadJsonFile(const std::string& _JsonFilePath)
		{
			return ReadStream(_JsonFilePath, this->Value);
		}
		bool ReadJsonFile(const std::string& _JsonFilePath, int32_t& _ErrorTips)
		{
			return ReadStream(_JsonFilePath, this->Value, _ErrorTips);
		}
		//读取 Json文件到 _Value
		bool ReadJsonFile(const std::string& _JsonFilePath, Json::Value& _JsonValue)
		{
			return ReadStream(_JsonFilePath, _JsonValue);
		}
		//写入 Value到 Json文件
		bool WriteJsonFile(std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(this->JsonFilePath, this->Value, _StreamOpenMode);
		}
		//写入 Value到 Json文件
		bool WriteJsonFile(const std::string& _JsonFilePath, std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(_JsonFilePath, this->Value, _StreamOpenMode);
		}
		//写入 _Value到 Json文件
		bool WriteJsonFile(const std::string& _JsonFilePath, Json::Value& _Value, std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(_JsonFilePath, _Value, _StreamOpenMode);
		}

	public:
		Json::Value GetJsonValue() const
		{
			return this->Value;
		}
		void SetJsonValue(Json::Value _Value)
		{
			this->Value = _Value;
		}
		Json::StreamWriterBuilder GetWriterBuilder() const
		{
			return this->WriterBuilder;
		}
		void SetWriterBuilder(Json::StreamWriterBuilder _WriterBuilder)
		{
			this->WriterBuilder = _WriterBuilder;
		}
		Json::CharReaderBuilder GetReaderBuilder() const
		{
			return this->ReaderBuilder;
		}
		void SetReaderBuilder(Json::CharReaderBuilder _ReaderBuilder)
		{
			this->ReaderBuilder = _ReaderBuilder;
		}
		std::string GetJsonFilePath() const
		{
			return this->JsonFilePath;
		}
		void SetJsonFilePath(std::string _JsonFilePath)
		{
			this->JsonFilePath = _JsonFilePath;
		}

	public:

		//输出 writeString到 Terr
		void ToStreamString(LogMessage (*_lm)() = Lnf)
		{
			LogDebug(stow(Json::writeString(this->WriterBuilder, this->Value)), _lm);
		}
	};
}






#endif