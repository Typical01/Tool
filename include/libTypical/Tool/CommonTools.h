#pragma once

//常用工具
#ifndef _COMMONTOOLS_H
#define _COMMONTOOLS_H

#include <libTypical/Tool/pch.h>
#include <libTypical/Tool/Log.h>
#include <libTypical/Tool/Time_Typical.h>

using namespace std;

namespace Typical_Tool {
	class FileSystem {
	private:
		std::filesystem::path Path;
		Log& log = lgc;

	public:
		FileSystem()
		{

		}
		FileSystem(const std::filesystem::path& _Path)
			: Path(_Path)
		{
		}

		FileSystem& operator/=(const Tstr& _PathName)
		{
			this->Path /= _PathName;
			return *this;
		}

	private:
		bool Exists(const Tstr& _TipsChar, const std::filesystem::path& _Path)
		{	
			Tstr FileType;
			if (std::filesystem::exists(_Path)) {
				if (std::filesystem::is_regular_file(_Path)) { // 检查路径是否是普通文件
					FileType = Tx("文件");
				}
				else if (std::filesystem::is_directory(_Path)){ // 检查路径是否是目录
					FileType = Tx("目录");
				}
				else {
					FileType = Tx("其他(符号链接...)");
				}

#ifdef UNICODE
				log(_TipsChar + Tx(":[") + _Path.wstring() + Tx("][") + FileType + Tx("] 存在!"), Err);
#else
				log(_TipsChar + Tx(":[") + _Path.string() + Tx("][") + FileType + Tx("] 存在!"), Err);
#endif
				return true;
			}
			else {
#ifdef UNICODE
				log(_TipsChar + Tx(":[") + _Path.wstring() + Tx("] 不存在!"), Tip);
#else
				log(_TipsChar + Tx(":[") + _Path.string() + Tx("] 不存在!"), Tip);
#endif
				return false;
			}
		}

	public:
		//创建目录: 自动递归创建下级目录
		bool CreateDirectorys()
		{
			if (Exists(Tx("创建文件夹"), this->Path)) {
				return false;
			}
#ifdef UNICODE
			auto PathSlash = this->Path.wstring().find_last_of(Tx("\\/"));
#else
			auto PathSlash = this->Path.string().find_last_of(Tx("\\/"));
#endif
			try {
				if (PathSlash == Tstr::npos) {
					//单级目录
					std::filesystem::create_directory(this->Path);
				}
				else {
					//多级目录
					std::filesystem::create_directories(this->Path);
				}
			}
			catch (const std::filesystem::filesystem_error& e) {
#ifdef UNICODE
				lgcr(Format(Tx("FileSystem::CreateDirectory: 失败原因: { % }"), stow(e.what()).c_str()), Err);
#else
				lgcr(Format(Tx("FileSystem::CreateDirectory: 失败原因: { % }"), e.what()), Err);
#endif
				return false;
			}
			catch (...) {
				lgcr(Tx("FileSystem::CreateDirectory: 其他错误!"), Err);
				return false;
			}

			return true;
		}
		//删除文件/目录
		bool Delete(bool _IsRecursive = false)
		{
			if (!Exists(Tx("删除文件/目录"), this->Path)) {
				return false;
			}

			try {
				if (_IsRecursive) {
					std::filesystem::remove_all(this->Path);
				}
				else {
					std::filesystem::remove(this->Path);
				}
			}
			catch (const std::filesystem::filesystem_error& e) {
#ifdef UNICODE
				lgcr(Format(Tx("FileSystem::Delete: 失败原因: { % }"), stow(e.what()).c_str()), Err);
#else
				lgcr(Format(Tx("FileSystem::Delete: 失败原因: { % }"), e.what()), Err);
#endif
				return false;
			}
			catch (...) {
				lgcr(Tx("FileSystem::Delete: 其他错误!"), Err);
				return false;
			}

			return true;
		}
		//复制文件/目录: _OnlyCopyNewFile 只复制最新时间的文件
		bool Copy(const Tstr& _TargetPath, bool _OnlyCopyNewFile = false)
		{
			if (!Exists(Tx("复制文件/目录"), this->Path) && !Exists(Tx("复制文件/目录"), _TargetPath)) {
				return false;
			}

			try {
				if (_OnlyCopyNewFile) {
					auto Source_time = std::filesystem::last_write_time(this->Path);
					auto Target_time = std::filesystem::last_write_time(_TargetPath);

					//仅复制新文件
					if (Source_time > Target_time) {
						std::filesystem::copy(this->Path, _TargetPath, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
					}
					else {
					}
				}
				else {
					std::filesystem::copy(this->Path, _TargetPath, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				}
			}
			catch (const std::filesystem::filesystem_error& e) {
#ifdef UNICODE
				lgcr(Format(Tx("FileSystem::Copy: 失败原因: { % }"), stow(e.what()).c_str()), Err);
#else
				lgcr(Format(Tx("FileSystem::Copy: 失败原因: { % }"), e.what()), Err);
#endif
				return false;
			}
			catch (...) {
				lgcr(Tx("FileSystem::Copy: 其他错误!"), Err);
				return false;
			}
			return true;
		}
		//重命名/移动 文件/目录
		bool ReName(const Tstr& _NewPathName)
		{
			std::filesystem::path NewPathName = _NewPathName;
			if (this->Path.parent_path() == NewPathName.parent_path()) {
				if (!Exists(Tx("重命名文件/目录"), this->Path)) {
					return false;
				}
			}
			else {
				if (!Exists(Tx("移动文件/目录"), this->Path)) {
					return false;
				}
			}

			try {
				std::filesystem::rename(this->Path, NewPathName);
			}
			catch (const std::filesystem::filesystem_error& e) {
#ifdef UNICODE
				lgcr(Format(Tx("FileSystem::ReName: 失败原因: { % }"), stow(e.what()).c_str()), Err);
#else
				lgcr(Format(Tx("FileSystem::ReName: 失败原因: { % }"), e.what()), Err);
#endif
				return false;
			}
			catch (...) {
				lgcr(Tx("FileSystem::ReName: 其他错误!"), Err);
				return false;
			}
			return true;
		}
		//修改文件/目录权限
		bool SetPermissions(const std::filesystem::perms& _perms)
		{
			if (!Exists(Tx("修改文件/目录权限"), this->Path)) {
				return false;
			}

			try {
				std::filesystem::permissions(this->Path, _perms);
			}
			catch (const std::filesystem::filesystem_error& e) {
#ifdef UNICODE
				lgcr(Format(Tx("FileSystem::SetPermissions: 失败原因: { % }"), stow(e.what()).c_str()), Err);
#else
				lgcr(Format(Tx("FileSystem::SetPermissions: 失败原因: { % }"), e.what()), Err);
#endif
				return false;
			}
			catch (...) {
				lgcr(Tx("FileSystem::SetPermissions: 其他错误!"), Err);
				return false;
			}
			return true;
		}

		//遍历目录: return 目录列表
		/* 
		* _IsRecursive: 是否递归
		* DirectoryList: 默认为空(不获取文件列表的字符串) | 非空(清楚内容后, 写入文件列表的字符串)
		*/
		std::vector<std::filesystem::directory_entry> DirectoryIterator(bool _IsRecursive = false, 
			std::shared_ptr<std::vector<std::filesystem::path>> DirectoryListStr = std::make_shared<std::vector<std::filesystem::path>>()
		) {
			std::vector<std::filesystem::directory_entry> List; //目录列表

			try {
				if (_IsRecursive) { // 递归遍历
					if (!DirectoryListStr->empty()) { //非空时: 获取文件列表的字符串
						DirectoryListStr->clear(); //清空

						for (const auto& entry : std::filesystem::recursive_directory_iterator(this->Path)) {
							DirectoryListStr->push_back(entry.path());
							List.push_back(entry);
						}
					}
					else {
						for (const auto& entry : std::filesystem::recursive_directory_iterator(this->Path)) {
							List.push_back(entry);
						}
					}
				}
				else { // 非递归遍历
					if (!DirectoryListStr->empty()) { //非空时: 获取文件列表的字符串
						DirectoryListStr->clear(); //清空

						for (const auto& entry : std::filesystem::directory_iterator(this->Path)) {
							DirectoryListStr->push_back(entry.path());
							List.push_back(entry);
						}
					}
					else {
						for (const auto& entry : std::filesystem::directory_iterator(this->Path)) {
							List.push_back(entry);
						}
					}
				}
			}
			catch (const std::filesystem::filesystem_error& e) {
#ifdef UNICODE
				lgcr(Format(Tx("FileSystem::DirectoryIterator: 失败原因: { % }"), stow(e.what()).c_str()), Err);
#else
				lgcr(Format(Tx("FileSystem::DirectoryIterator: 失败原因: { % }"), e.what()), Err);
#endif
				return std::vector< std::filesystem::directory_entry>();
			}
			catch (...) {
				lgcr(Tx("FileSystem::DirectoryIterator: 其他错误!"), Err);
				return std::vector< std::filesystem::directory_entry>();
			}

			return List;
		}

	public:
		//设置 std::filesystem::path
		void SetPath(const std::filesystem::path& _Path) { this->Path = _Path; }
		void SetLog(Log& _Log) { this->log = _Log; }

	public:
		//获取 当前工作路径
	    static Tstr GetCurrentPath() { 
#ifdef UNICODE
			return std::filesystem::current_path().wstring();
#else
			return std::filesystem::current_path().string();
#endif 
		}
		//获取 std::filesystem::path
		std::filesystem::path GetPath() { return this->Path; }
		//获取 文件名
		Tstr GetFileName() const {
#ifdef UNICODE
			return this->Path.filename().wstring();
#else
			return this->Path.filename().string();
#endif 
		}
		//获取 文件扩展名
		Tstr GetExtensionName() const { 
#ifdef UNICODE
			return this->Path.extension().wstring();
#else
			return this->Path.extension().string();
#endif
		}
		//获取 父级目录名
		Tstr GetParentName() const { 
#ifdef UNICODE
			return this->Path.parent_path().wstring();
#else
			return this->Path.parent_path().string();
#endif	
		}
	};

	namespace GameTools
	{
		//帧率-----------------------------------------------------------------------------------------------------------

#ifndef FPS_COUNT
#define FPS_COUNT 60 //Fps 统计的间隔(帧率)
#endif
		//获取 FPS
		template<class T = bool>
		float GetFps() {
			//临时统计
			static int tempCount = 0;
			static Timer timer; //时间
			static float fps; //帧率

			if (tempCount > FPS_COUNT) {
				//将时间节点拓展到两个
				if (timer.GetTimerSize() < 2) {
					timer.AddTimer();
				}

				tempCount = 0;
				timer.SetTimer(timer.GetTime(), 2);
				float tempTime = (float)timer.ComputTime_FirstToEnd();
				fps = FPS_COUNT / (tempTime / 1000.0f); //获取的系统时间为毫秒数
				timer.SetTimer(timer.GetTime(), 1);
			}

			tempCount++;
			return fps;
		}
	}
}

#endif