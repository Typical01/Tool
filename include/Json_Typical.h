#ifndef _JSON_TYPICAL_H
#define _JSON_TYPICAL_H

#include "pch.h"
#include "Json/json.h"
#include "Log.h"

namespace Typical_Tool {

	class JsonManage {
	private:
		Json::Value Value;
		Json::StreamWriterBuilder WriterBuilder;
		Json::CharReaderBuilder ReaderBuilder;
		Tstr JsonFilePath;
		std::reference_wrapper<Log> log = lg;

	public:
		JsonManage()
		{}
		JsonManage(const Tstr& _JsonFilePath, bool _IsReadJsonFile = false)
		{
			this->Init(_JsonFilePath, _IsReadJsonFile);
		}

		//Json写入流配置: utf-8非英文字符显示 | 缩进 | 保存文件路径 | 读取并解析Json文件(return bool)
		bool Init(const Tstr& _JsonFilePath, bool _IsReadJsonFile)
		{
			this->WriterBuilder[_T("emitUTF8")] = true; //utf-8字符显示为非 /uxxx
			this->WriterBuilder[_T("indentation")] = _T("    "); // 设置缩进
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
				JsonFileOut << Json::writeString(this->WriterBuilder, _Value);
				return true;
			}
			else {
				log(_T("打开Json文件失败: ofstream"), er);
				log(_T("\tJson文件路径: ") + _Bracket(_JsonFilePath), er);
				return false;
			}
		}
		bool ReadStream(const Tstr& _JsonFilePath, Json::Value& _Value)
		{
			Tifstream JsonFile(_JsonFilePath, Tifstream::binary);
			Tstr ErrorCode;
			if (JsonFile.is_open()) {
				if (Json::parseFromStream(this->ReaderBuilder, JsonFile, &_Value, &ErrorCode)) {
					return true;
				}
				else {
					log(_T("解析 Json失败: ") + ErrorCode, er);
					log(_T("\tJson文件路径: ") + _Bracket(_JsonFilePath), er);
					return false;
				}
			}
			else {
				log(_T("打开Json文件失败: ") + ErrorCode, er);
				log(_T("\tJson文件路径: ") + _Bracket(_JsonFilePath), er);
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
		//写入 Value到 Json文件: this->Value
		bool WriteJsonFile(std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(this->JsonFilePath, this->Value, _StreamOpenMode);
		}
		//写入 Value到 Json文件: this->Value
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
		void ToStreamString(LogMessage (*_lm)() = lf)
		{
			lgc(Json::writeString(this->WriterBuilder, this->Value), _lm);
		}

		//字符串转 Json::Value
		bool CharToJsonValue(const Tstr& _JsonConfig)
		{
			Tistringstream StrSream(_JsonConfig);
			Tstr ErrorCode;
			if (Json::parseFromStream(this->ReaderBuilder, StrSream, &this->Value, &ErrorCode)) {
				return true;
			}
			else {
				log(_T("转换字符串到 Json失败: ") + ErrorCode, er);
				log(_T("\tJson字符串: ") + _Bracket(_JsonConfig), er);
				return false;
			}
		}
	};
}






#endif