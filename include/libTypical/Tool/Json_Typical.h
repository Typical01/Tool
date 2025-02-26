#ifndef _JSON_TYPICAL_H
#define _JSON_TYPICAL_H

#include <libTypical/Tool/pch.h>
#include <Json/json.h>
#include <libTypical/Tool/Log.h>

namespace Typical_Tool {

	class JsonManage {
	private:
		Json::Value Value;
		Json::StreamWriterBuilder WriterBuilder;
		Json::CharReaderBuilder ReaderBuilder;
		Tstr JsonFilePath;
		Log& log = lgc;

	public:
		JsonManage()
		{}
		//Json写入流配置: utf-8非英文字符显示 | 缩进 | 保存文件路径 | 读取并解析Json文件(return bool)
		JsonManage(const Tstr& _JsonFilePath, bool _IsReadJsonFile = false)
		{
			this->Init(_JsonFilePath, _IsReadJsonFile);
		}

		//Json写入流配置: utf-8非英文字符显示 | 缩进 | 保存文件路径 | 读取并解析Json文件(return bool)
		bool Init(const Tstr& _JsonFilePath, bool _IsReadJsonFile)
		{
			this->WriterBuilder["emitUTF8"] = true; //utf-8字符显示为非 /uxxx
			this->WriterBuilder["indentation"] = "    "; // 设置缩进
			this->JsonFilePath = _JsonFilePath; //保存 Json文件路径

			if (_IsReadJsonFile) {
				return ReadJsonFile(_JsonFilePath);
			}
			else {
				return true;
			}
		}

	private:
		bool WriteStream(const Tstr& _JsonFilePath, Json::Value& _Value, std::ios::ios_base::openmode _StreamOpenMode)
		{
			Tofstream JsonFileOut(_JsonFilePath, _StreamOpenMode);
			if (JsonFileOut.is_open()) {
#ifdef UNICODE
				JsonFileOut << stow(Json::writeString(this->WriterBuilder, _Value));
#else
				JsonFileOut << Json::writeString(this->WriterBuilder, _Value);
#endif
				
				return true;
			}
			else {
				log(Tx("打开Json文件失败: !ofstream.is_open()"), Err);
				log(Tx("\tJson文件路径: ") + _Bracket(_JsonFilePath), Err);
				return false;
			}
		}
		bool ReadStream(const Tstr& _JsonFilePath, Json::Value& _Value)
		{
#ifdef UNICODE
			std::ifstream JsonFile(wtos(_JsonFilePath), Tifstream::binary);
#else
			std::ifstream JsonFile(_JsonFilePath, ifstream::binary);
#endif
			std::string ErrorCode;
			if (JsonFile.is_open()) {
#ifdef UNICODE
				if (Json::parseFromStream(this->ReaderBuilder, JsonFile, &_Value, &ErrorCode)) {
#else
				if (Json::parseFromStream(this->ReaderBuilder, JsonFile, &_Value, &ErrorCode)) {
#endif
					return true;
				}
				else {
#ifdef UNICODE
					log(Format(Tx("解析 Json失败: [%]"), stow(ErrorCode)), Err);
#else
					log(Format(Tx("解析 Json失败: [%]"), ErrorCode), Err);
#endif
					log(Format(Tx("\tJson文件路径: [%]"), _Bracket(_JsonFilePath)), Err);
					return false;
				}
			}
			else {
#ifdef UNICODE
				log(Format(Tx("打开Json文件失败: [%]"), stow(ErrorCode)), Err);
#else
				log(Format(Tx("打开Json文件失败: [%]"), ErrorCode), Err);
#endif
				log(Format(Tx("\tJson文件路径: [%]"), _Bracket(_JsonFilePath)), Err);
				return false;
			}
		}

	public:
		//读取 Json文件到 _Value
		bool ReadJsonFile()
		{
			return ReadStream(this->JsonFilePath, this->Value);
		}
		//读取 Json文件到 _Value
		bool ReadJsonFile(const Tstr& _JsonFilePath)
		{
			return ReadStream(_JsonFilePath, this->Value);
		}
		//读取 Json文件到 _Value
		bool ReadJsonFile(const Tstr& _JsonFilePath, Json::Value& _JsonValue)
		{
			return ReadStream(_JsonFilePath, _JsonValue);
		}
		//写入 Value到 Json文件
		bool WriteJsonFile(std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(this->JsonFilePath, this->Value, _StreamOpenMode);
		}
		//写入 Value到 Json文件
		bool WriteJsonFile(const Tstr& _JsonFilePath, std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(_JsonFilePath, this->Value, _StreamOpenMode);
		}
		//写入 _Value到 Json文件
		bool WriteJsonFile(const Tstr& _JsonFilePath, Json::Value& _Value, std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
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
		Tstr GetJsonFilePath() const
		{
			return this->JsonFilePath;
		}
		void SetJsonFilePath(Tstr _JsonFilePath)
		{
			this->JsonFilePath = _JsonFilePath;
		}

		void SetLog(Log& _log) { this->log = _log; }

	public:

		//输出 writeString到 Terr
		void ToStreamString(LogMessage (*_lm)() = Lnf)
		{
#ifdef UNICODE
			log(stow(Json::writeString(this->WriterBuilder, this->Value)), _lm);
#else
			log(Json::writeString(this->WriterBuilder, this->Value), _lm);
#endif
		}
	};
}






#endif