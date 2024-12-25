#include "StringManage.h"


std::wstring Typical_Tool::StringManage::StringToWstring(const std::string& str)
{
	std::wstring wContext;

	if (str.empty()) {
		return wContext;
	}

#ifdef _WINDOWS
	// Windows 版本
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
	if (len <= 0) {
		throw std::runtime_error("Failed to convert string to wide string.");
	}
	std::unique_ptr<wchar_t[]> buffer(new wchar_t[len + 1]);
	if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), buffer.get(), len) <= 0) {
		throw std::runtime_error("Failed to convert string to wide string.");
	}
	buffer[len] = _T('\0');
	wContext.assign(buffer.get());
#else
	if (IsRunTimeError) {
		throw std::runtime_error("StringToWstring: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)");
	}
#endif

	return wContext;
}

std::wstring Typical_Tool::StringManage::StringToWstring(std::string&& str)
{
	std::wstring wContext;

	if (str.empty()) {
		return wContext;
	}

#ifdef _WINDOWS
	// Windows 版本
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
	if (len <= 0) {
		throw std::runtime_error("Failed to convert string to wide string.");
	}
	std::unique_ptr<wchar_t[]> buffer(new wchar_t[len + 1]);
	if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), buffer.get(), len) <= 0) {
		throw std::runtime_error("Failed to convert string to wide string.");
	}
	buffer[len] = _T('\0');
	wContext.assign(buffer.get());
#else
	if (IsRunTimeError) {
		throw std::runtime_error("StringToWstring: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)");
	}
#endif

	return wContext;
}

std::string Typical_Tool::StringManage::WstringToString(const std::wstring& wStr)
{
	std::string context;

	if (wStr.empty()) {
		return context;
	}

#ifdef _WINDOWS
	// Windows 版本
	int len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (len <= 0) {
		throw std::runtime_error("Failed to convert wide string to string.");
	}
	std::unique_ptr<char[]> buffer(new char[len + 1]);
	if (WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, buffer.get(), len, nullptr, nullptr) <= 0) {
		throw std::runtime_error("Failed to convert wide string to string.");
	}
	buffer[len] = '\0';
	context.assign(buffer.get());
#else
	if (IsRunTimeError) {
		throw std::runtime_error("WstringToString: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)");
	}
#endif

	return context;
}

std::string Typical_Tool::StringManage::WstringToString(std::wstring&& wStr)
{
	std::string context;

	if (wStr.empty()) {
		return context;
	}

#ifdef _WINDOWS
	// Windows 版本
	int len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (len <= 0) {
		throw std::runtime_error("Failed to convert wide string to string.");
	}
	std::unique_ptr<char[]> buffer(new char[len + 1]);
	if (WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, buffer.get(), len, nullptr, nullptr) <= 0) {
		throw std::runtime_error("Failed to convert wide string to string.");
	}
	buffer[len] = '\0';
	context.assign(buffer.get());
#else
	if (IsRunTimeError) {
		throw std::runtime_error("WstringToString: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)");
	}
#endif

	return context;
}
