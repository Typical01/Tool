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
		Log& LogDebug = lgc;
		Log& LogRelease = lgcr;

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
		/*
		* _TipsChar: 提示文本
		* _Path: 检查的路径
		* _bShowRelease: 输出到 Release
		* _bFileTrue: 文件存在时为真, 否则取反
		*/
		bool Exists(const Tstr& _TipsChar, const std::filesystem::path& _Path, bool _bShowRelease = false, bool _bFileTrue = true)
		{
			Tstr FileType;
			if (std::filesystem::exists(_Path)) {
				if (std::filesystem::is_regular_file(_Path)) { // 检查路径是否是普通文件
					FileType = TEXT("文件");
				}
				else if (std::filesystem::is_directory(_Path)) { // 检查路径是否是目录
					FileType = TEXT("目录");
				}
				else {
					FileType = TEXT("其他(符号链接...)");
				}

				if (_bShowRelease) {
					if (_bFileTrue) {
						LogRelease(_TipsChar + TEXT(":[") + _Path.PathToStr() + TEXT("][") + FileType + TEXT("] 存在!"), Tip);
					}
					else {
						LogRelease(_TipsChar + TEXT(":[") + _Path.PathToStr() + TEXT("][") + FileType + TEXT("] 存在!"), Err);
					}
					return true;
				}
				else {
					if (_bFileTrue) {
						LogDebug(_TipsChar + TEXT(":[") + _Path.PathToStr() + TEXT("][") + FileType + TEXT("] 存在!"), Tip);
					}
					else {
						LogDebug(_TipsChar + TEXT(":[") + _Path.PathToStr() + TEXT("][") + FileType + TEXT("] 存在!"), Err);
					}
					return true;
				}
			}
			else {
				if (_bShowRelease) {
					if (_bFileTrue) {
						LogRelease(_TipsChar + TEXT(":[") + _Path.PathToStr() + TEXT("] 不存在!"), Err);
					}
					else {
						LogRelease(_TipsChar + TEXT(":[") + _Path.PathToStr() + TEXT("] 不存在!"), Tip);
					}
					return false;
				}
				else {
					if (_bFileTrue) {
						LogDebug(_TipsChar + TEXT(":[") + _Path.PathToStr() + TEXT("] 不存在!"), Err);
					}
					else {
						LogDebug(_TipsChar + TEXT(":[") + _Path.PathToStr() + TEXT("] 不存在!"), Tip);
					}
					return false;
				}
			}
		}

	public:
		//创建目录: 自动递归创建下级目录
		bool CreateDirectorys()
		{
			if (Exists(TEXT("创建文件夹"), this->Path)) {
				return false;
			}
#ifdef UNICODE
			auto PathSlash = this->Path.PathToStr().find_last_of(TEXT("\\/"));
#else
			auto PathSlash = this->Path.PathToStr().find_last_of(TEXT("\\/"));
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
			catch (const std::filesystem::filesystem_error& Error) {
				LogRelease(Printf(TEXT("FileSystem::CreateDirectory: 失败原因: { %s }"), stow(Error.what())), Err);
				LogRelease(Printf(TEXT("FileSystem::CreateDirectory: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"), 
					Error.path1().PathToStr(), Error.path2().PathToStr()), War);
				Exists(TEXT("创建文件夹(Path1)"), Error.path1(), true, false);
				Exists(TEXT("创建文件夹(Path2)"), Error.path2(), true, false);

				return false;
			}
			catch (...) {
				LogRelease(TEXT("FileSystem::CreateDirectory: 其他错误!"), Err);
				return false;
			}

			return true;
		}
		//删除文件/目录
		bool Delete(bool _IsRecursive = false)
		{
			if (!Exists(TEXT("删除文件/目录"), this->Path)) {
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
			catch (const std::filesystem::filesystem_error& Error) {
				LogRelease(Printf(TEXT("FileSystem::Delete: 失败原因: { %s }"), stow(Error.what())), Err);
				LogRelease(Printf(TEXT("FileSystem::Delete: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), War);
				Exists(TEXT("删除文件/目录(Path1)"), Error.path1(), true);
				Exists(TEXT("删除文件/目录(Path2)"), Error.path2(), true);

				return false;
			}
			catch (...) {
				LogRelease(TEXT("FileSystem::Delete: 其他错误!"), Err);
				return false;
			}

			return true;
		}
		//复制文件/目录: _OnlyCopyNewFile 只复制最新时间的文件
		bool Copy(const Tstr& _TargetPath, bool _OnlyCopyNewFile = false)
		{
			if (!Exists(TEXT("复制文件/目录"), this->Path) && !Exists(TEXT("复制文件/目录"), _TargetPath)) {
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
			catch (const std::filesystem::filesystem_error& Error) {
				LogRelease(Printf(TEXT("FileSystem::Copy: 失败原因: { %s }"), stow(Error.what()).c_str()), Err);
				LogRelease(Printf(TEXT("FileSystem::Copy: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), War);
				Exists(TEXT("复制文件/目录(Path1)"), Error.path1(), true);
				Exists(TEXT("复制文件/目录(Path2)"), Error.path2(), true);

				return false;
			}
			catch (...) {
				LogRelease(TEXT("FileSystem::Copy: 其他错误!"), Err);
				return false;
			}
			return true;
		}
		//重命名/移动 文件/目录
		bool ReName(const Tstr& _NewPathName)
		{
			std::filesystem::path NewPathName = _NewPathName;
			if (this->Path.parent_path() == NewPathName.parent_path()) {
				if (!Exists(TEXT("重命名文件/目录"), this->Path)) {
					return false;
				}

				try {
					std::filesystem::rename(this->Path, NewPathName);
				}
				catch (const std::filesystem::filesystem_error& Error) {
					LogRelease(Printf(TEXT("FileSystem::ReName: 失败原因: { %s }"), stow(Error.what()).c_str()), Err);
					LogRelease(Printf(TEXT("FileSystem::ReName: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
						Error.path1().PathToStr(), Error.path2().PathToStr()), War);
					Exists(TEXT("重命名文件/目录(Path1)"), Error.path1(), true);
					Exists(TEXT("重命名文件/目录(Path2)"), Error.path2(), true);

					return false;
				}
				catch (...) {
					LogRelease(TEXT("FileSystem::ReName: 其他错误!"), Err);
					return false;
				}
			}
			else {
				if (!Exists(TEXT("移动文件/目录"), this->Path)) {
					return false;
				}

				try {
					std::filesystem::rename(this->Path, NewPathName);
				}
				catch (const std::filesystem::filesystem_error& Error) {
					LogRelease(Printf(TEXT("FileSystem::ReName: 失败原因: { %s }"), stow(Error.what()).c_str()), Err);
					LogRelease(Printf(TEXT("FileSystem::ReName: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
						Error.path1().PathToStr(), Error.path2().PathToStr()), War);
					Exists(TEXT("移动文件/目录(Path1)"), Error.path1(), true);
					Exists(TEXT("移动文件/目录(Path2)"), Error.path2(), true);

					return false;
				}
				catch (...) {
					LogRelease(TEXT("FileSystem::ReName: 其他错误!"), Err);
					return false;
				}
			}
			
			return true;
		}
		//修改文件/目录权限
		bool SetPermissions(const std::filesystem::perms& _perms)
		{
			if (!Exists(TEXT("修改文件/目录权限"), this->Path)) {
				return false;
			}

			try {
				std::filesystem::permissions(this->Path, _perms);
			}
			catch (const std::filesystem::filesystem_error& Error) {
				LogRelease(Printf(TEXT("FileSystem::SetPermissions: 失败原因: { %s }"), stow(Error.what()).c_str()), Err);
				LogRelease(Printf(TEXT("FileSystem::SetPermissions: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), War);
				Exists(TEXT("修改文件/目录权限(Path1)"), Error.path1(), true);
				Exists(TEXT("修改文件/目录权限(Path2)"), Error.path2(), true);

				return false;
			}
			catch (...) {
				LogRelease(TEXT("FileSystem::SetPermissions: 其他错误!"), Err);
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
			catch (const std::filesystem::filesystem_error& Error) {
				LogRelease(Printf(TEXT("FileSystem::DirectoryIterator: 失败原因: { %s }"), stow(Error.what()).c_str()), Err);
				LogRelease(Printf(TEXT("FileSystem::DirectoryIterator: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), War);
				Exists(TEXT("遍历目录(Path1)"), Error.path1(), true);
				Exists(TEXT("遍历目录(Path2)"), Error.path2(), true);

				return std::vector< std::filesystem::directory_entry>();
			}
			catch (...) {
				LogRelease(TEXT("FileSystem::DirectoryIterator: 其他错误!"), Err);
				return std::vector< std::filesystem::directory_entry>();
			}

			return List;
		}

		//遍历目录: return 目录列表
		/*
		* _IsRecursive: 是否递归
		* DirectoryList: 默认为空(不获取文件列表的字符串) | 非空(清楚内容后, 写入文件列表的字符串)
		*/
		std::vector<std::filesystem::directory_entry> DirectoryIterator(bool _IsRecursive, __int64& _FilePathSizeSum,
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
							_FilePathSizeSum += entry.file_size();
						}
					}
					else {
						for (const auto& entry : std::filesystem::recursive_directory_iterator(this->Path)) {
							List.push_back(entry);
							_FilePathSizeSum += entry.file_size();
						}
					}
				}
				else { // 非递归遍历
					if (!DirectoryListStr->empty()) { //非空时: 获取文件列表的字符串
						DirectoryListStr->clear(); //清空

						for (const auto& entry : std::filesystem::directory_iterator(this->Path)) {
							DirectoryListStr->push_back(entry.path());
							List.push_back(entry);
							_FilePathSizeSum += entry.file_size();
						}
					}
					else {
						for (const auto& entry : std::filesystem::directory_iterator(this->Path)) {
							List.push_back(entry);
							_FilePathSizeSum += entry.file_size();
						}
					}
				}
			}
			catch (const std::filesystem::filesystem_error& Error) {
				LogRelease(Printf(TEXT("FileSystem::DirectoryIterator: 失败原因: { %s }"), stow(Error.what()).c_str()), Err);
				LogRelease(Printf(TEXT("FileSystem::DirectoryIterator: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), War);
				Exists(TEXT("遍历目录(Path1)"), Error.path1(), true);
				Exists(TEXT("遍历目录(Path2)"), Error.path2(), true);

				return std::vector< std::filesystem::directory_entry>();
			}
			catch (...) {
				LogRelease(TEXT("FileSystem::DirectoryIterator: 其他错误!"), Err);
				return std::vector< std::filesystem::directory_entry>();
			}

			return List;
		}

	public:
		//设置 std::filesystem::path
		void SetPath(const std::filesystem::path& _Path) { this->Path = _Path; }
		void SetLog(Log& _Log) { this->LogDebug = _Log; }

	public:
		//获取 当前工作路径: 可能变化
	    static Tstr GetCurrentPath() { 
			return std::filesystem::current_path().PathToStr();
		}
		//获取 std::filesystem::path
		std::filesystem::path GetPath() { return this->Path; }
		//获取 文件名
		Tstr GetFileName() const {
			return this->Path.filename().PathToStr();
		}
		//获取 文件扩展名
		Tstr GetExtensionName() const { 
			return this->Path.extension().PathToStr();
		}
		//获取 父级目录名
		Tstr GetParentName() const { 
			return this->Path.parent_path().PathToStr();
		}
	};



	class ThreadPool {
	public:
		// 构造函数，创建指定数量的工作线程
		ThreadPool(size_t _NumThreads) : StopThread(false) {
			for (size_t i = 0; i < _NumThreads; ++i) {
				Workers.emplace_back([this] {
					while (true) {
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(QueueMutex);
							Condition.wait(lock, [this] { return StopThread || !Tasks.empty(); });
							if (StopThread && Tasks.empty()) return;
							task = std::move(Tasks.front());
							Tasks.pop();
						}
						task();
					}
					});
			}
		}

		// 析构函数，停止线程池
		~ThreadPool() {
			JoinAll();
		}

		// 提交任务到线程池
		template <class F, class... Args>
		auto Enqueue(F&& _Func, Args&&... _Args) -> std::future<typename std::invoke_result<F(Args...)>::type> {
			using return_type = typename std::invoke_result<F(Args...)>::type;

			auto task = std::make_shared<std::packaged_task<return_type()>>(
				std::bind(std::forward<F>(_Func), std::forward<Args>(_Args)...)
			);

			std::future<return_type> res = task->get_future();
			{
				std::unique_lock<std::mutex> lock(QueueMutex);
				if (StopThread) {
					throw wruntime_error(_LOGERRORINFO(TEXT("tytl::ThreadPool: enqueue on stopped ThreadPool!")));
				}
				Tasks.emplace([task]() { (*task)(); });
			}
			Condition.notify_one();
			return res;
		}

		/*
		* 当前任务数量
		*/
		__int32 WorkNumber() {
			{
				std::unique_lock<std::mutex> lock(QueueMutex);
				return Tasks.size();
			}
		}

		/*
		* 主动连接所有线程
		*/
		void JoinAll() {
			{
				std::unique_lock<std::mutex> lock(QueueMutex);
				StopThread = true;
			}
			Condition.notify_all();
			for (auto& tempThread : Workers) {
				if (tempThread.joinable()) {
					tempThread.join();
				}
			}
		}

	private:
		std::vector<std::thread> Workers; // 工作线程
		std::queue<std::function<void()>> Tasks; // 任务队列
		std::mutex QueueMutex; // 保护任务队列的互斥锁
		std::condition_variable Condition; // 条件变量，用于线程同步
		bool StopThread; // 停止标志
	};
}

#endif