#pragma once

//常用工具
#ifndef _COMMONTOOLS_H
#define _COMMONTOOLS_H

#include "pch.h"
#include "Log.h"
#include "Time_Typical.h"

using namespace std;

namespace Typical_Tool {
	class FileSystem {
	private:
		std::filesystem::path Path;
		std::reference_wrapper<Log> log = lgc;

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
					FileType = _T("文件");
				}
				else if (std::filesystem::is_directory(_Path)){ // 检查路径是否是目录
					FileType = _T("目录");
				}
				else {
					FileType = _T("其他(符号链接...)");
				}

				log(_TipsChar + _T(":[") + _Path.string() + _T("][") + FileType + _T("] 存在!"), er);
				return true;
			}
			else {
				log(_TipsChar + _T(":[") + _Path.string() + _T("] 不存在!"), ts);
				return false;
			}
		}

	public:
		//创建目录: 自动递归创建下级目录
		bool CreateDirectory()
		{
			if (Exists(_T("创建文件夹"), this->Path)) {
				return false;
			}
			auto PathSlash = this->Path.string().find_last_of(_T("\\/"));
			try {
				if (PathSlash == std::string::npos) {
					//单级目录
					std::filesystem::create_directory(this->Path);
				}
				else {
					//多级目录
					std::filesystem::create_directories(this->Path);
				}
			}
			catch (const std::filesystem::filesystem_error& e) {
				lgcr((Tstr)"FileSystem::CreateDirectory: 失败原因: " + e.what(), er);
				return false;
			}
			catch (...) {
				lgcr("FileSystem::CreateDirectory: 其他错误!", er);
				return false;
			}

			return true;
		}
		//删除文件/目录
		bool Delete(bool _IsRecursive = false)
		{
			if (!Exists(_T("删除文件/目录"), this->Path)) {
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
				lgcr((Tstr)"FileSystem::Delete: 失败原因: " + e.what(), er);
				return false;
			}
			catch (...) {
				lgcr("FileSystem::Delete: 其他错误!", er);
				return false;
			}

			return true;
		}
		//复制文件/目录
		bool Copy(const Tstr& _TargetPath)
		{
			if (!Exists(_T("复制文件/目录"), this->Path) && !Exists(_T("复制文件/目录"), _TargetPath)) {
				return false;
			}

			try {
				std::filesystem::copy(this->Path, _TargetPath, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (const std::filesystem::filesystem_error& e) {
				lgcr((Tstr)"FileSystem::Copy: 失败原因: " + e.what(), er);
				return false;
			}
			catch (...) {
				lgcr("FileSystem::Copy: 其他错误!", er);
				return false;
			}
			return true;
		}
		//重命名/移动 文件/目录
		bool ReName(const Tstr& _NewPathName)
		{
			std::filesystem::path NewPathName = _NewPathName;
			if (this->Path.parent_path() == NewPathName.parent_path()) {
				if (!Exists(_T("重命名文件/目录"), this->Path)) {
					return false;
				}
			}
			else {
				if (!Exists(_T("移动文件/目录"), this->Path)) {
					return false;
				}
			}

			try {
				std::filesystem::rename(this->Path, NewPathName);
			}
			catch (const std::filesystem::filesystem_error& e) {
				lgcr((Tstr)"FileSystem::ReName: 失败原因: " + e.what(), er);
				return false;
			}
			catch (...) {
				lgcr("FileSystem::ReName: 其他错误!", er);
				return false;
			}
			return true;
		}
		//修改文件/目录权限
		bool SetPermissions(const std::filesystem::perms& _perms)
		{
			if (!Exists(_T("修改文件/目录权限"), this->Path)) {
				return false;
			}

			try {
				std::filesystem::permissions(this->Path, _perms);
			}
			catch (const std::filesystem::filesystem_error& e) {
				lgcr((Tstr)"FileSystem::SetPermissions: 失败原因: " + e.what(), er);
				return false;
			}
			catch (...) {
				lgcr("FileSystem::SetPermissions: 其他错误!", er);
				return false;
			}
			return true;
		}

		//遍历目录: return 目录列表
		// *_IsRecursive: 是否递归
		std::vector<std::filesystem::path> DirectoryIterator(bool _IsRecursive = false)
		{
			std::vector<std::filesystem::path> List; //目录列表

			try {
				if (_IsRecursive) { // 递归遍历
					for (const auto& entry : std::filesystem::recursive_directory_iterator(this->Path)) {
						List.push_back(entry.path());
					}
				}
				else { // 非递归遍历
					for (const auto& entry : std::filesystem::directory_iterator(this->Path)) {
						List.push_back(entry.path());
					}
				}
			}
			catch (const std::filesystem::filesystem_error& e) {
				lgcr((Tstr)"FileSystem::DirectoryIterator: 失败原因: " + e.what(), er);
				return std::vector<std::filesystem::path>();
			}
			catch (...) {
				lgcr("FileSystem::DirectoryIterator: 其他错误!", er);
				return std::vector<std::filesystem::path>();
			}

			return List;
		}
	public:
		//设置 std::filesystem::path
		void SetPath(const std::filesystem::path& _Path) { this->Path = _Path; }
		void SetLog(Log& _log) { this->log = _log; }

	public:
		//获取 std::filesystem::path
		std::filesystem::path GetPath() { return this->Path; }
		//获取 文件名
		Tstr GetFileName() const { return this->Path.filename().string(); }
		//获取 文件扩展名
		Tstr GetExtensionName() const { return this->Path.extension().string(); }
		//获取 父级目录名
		Tstr GetParentName() const { return this->Path.parent_path().string(); }
	};

	namespace GameTools
	{
		//帧率-----------------------------------------------------------------------------------------------------------

#ifndef FPS_COUNT
#define FPS_COUNT 60 //Fps 统计的间隔(帧率)
#endif
		//获取 FPS
		template<class T = bool>
		float GetFps()
		{
			//临时统计
			static int tempCount = 0;
			static Timer timer; //时间
			static float fps; //帧率

			if (tempCount > FPS_COUNT)
			{
				//将时间节点拓展到两个
				if (timer.GetTimerSize() < 2)
					timer.AddTimer();

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