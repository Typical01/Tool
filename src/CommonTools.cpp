#include "pch.h"

#include "CommonTools.h"
#include "Message.h"


void Typical_Tool::WindowsSystem::SetDisplaySize(int displayWidth, int displayHeight)
{
	//初始化
	DEVMODE NewDevMode;
	EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &NewDevMode);

	//记录修改信息
	NewDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	NewDevMode.dmPelsWidth = displayWidth;
	NewDevMode.dmPelsHeight = displayHeight;

	//根据修改信息 修改屏幕分辨率
	ChangeDisplaySettings(&NewDevMode, 0);
}

 bool Typical_Tool::WindowsSystem::IsUserAdmin()
{
	BOOL retVal = FALSE;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	BOOL result = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup);

	if (result)
	{
		if (!CheckTokenMembership(NULL, AdministratorsGroup, &retVal))
		{
			retVal = FALSE;
		}
		FreeSid(AdministratorsGroup);
	}
	return retVal;
}

 void Typical_Tool::WindowsSystem::MoveCursorLocation(int x, int y) {
	 COORD pos = { x,y };
	 HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	 SetConsoleCursorPosition(hOut, pos);
 }

std::wstring Typical_Tool::StringHandling::StringToWstring(const std::string& str)
{
	std::wstring wContext;

	if (str.empty()) {
		return wContext;
	}

#ifdef _WINDOWS
	// Windows 版本
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);
	if (len <= 0) {
		throw std::runtime_error("Failed to convert string to wide string.");
	}
	std::unique_ptr<wchar_t[]> buffer(new wchar_t[len + 1]);
	if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), buffer.get(), len) <= 0) {
		throw std::runtime_error("Failed to convert string to wide string.");
	}
	buffer[len] = _T('\0');
	wContext.assign(buffer.get());
#elif _UNIX
	// POSIX 版本
	std::unique_ptr<char[]> buffer(new char[str.size() * 4]);  // 大小预留空间
	size_t out_len = str.size() * 4;
	iconv_t cd = iconv_open("UTF-16LE", "ISO-8859-1");
	if (cd == (iconv_t)-1) {
		throw std::runtime_error("Failed to open iconv descriptor.");
	}
	if (iconv(cd, const_cast<const char**>(&str.c_str()), &str.size(), reinterpret_cast<char**>(buffer.get()), &out_len) == (size_t)-1) {
		iconv_close(cd);
		throw std::runtime_error("Failed to convert string using iconv.");
	}
	iconv_close(cd);
	wContext.assign(reinterpret_cast<wchar_t*>(buffer.get()));
#else
	lgcr("stow: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)", lgm::er);
#endif

	return wContext;
}

std::wstring Typical_Tool::StringHandling::StringToWstring(std::string&& str)
{
	std::wstring wContext;

	if (str.empty()) {
		return wContext;
	}

#ifdef _WINDOWS
	// Windows 版本
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);
	if (len <= 0) {
		throw std::runtime_error("Failed to convert string to wide string.");
	}
	std::unique_ptr<wchar_t[]> buffer(new wchar_t[len + 1]);
	if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), buffer.get(), len) <= 0) {
		throw std::runtime_error("Failed to convert string to wide string.");
	}
	buffer[len] = _T('\0');
	wContext.assign(buffer.get());
#elif _UNIX
	// POSIX 版本
	std::unique_ptr<char[]> buffer(new char[str.size() * 4]);  // 大小预留空间
	size_t out_len = str.size() * 4;
	iconv_t cd = iconv_open("UTF-16LE", "ISO-8859-1");
	if (cd == (iconv_t)-1) {
		throw std::runtime_error("Failed to open iconv descriptor.");
	}
	if (iconv(cd, const_cast<const char**>(&str.c_str()), &str.size(), reinterpret_cast<char**>(buffer.get()), &out_len) == (size_t)-1) {
		iconv_close(cd);
		throw std::runtime_error("Failed to convert string using iconv.");
	}
	iconv_close(cd);
	wContext.assign(reinterpret_cast<wchar_t*>(buffer.get()));
#else
	lgcr("stow: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)", lgm::er);
#endif

	return wContext;
}

std::string Typical_Tool::StringHandling::WstringToString(const std::wstring& wStr)
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
#elif _UNIX
	// POSIX 版本
	std::unique_ptr<char[]> buffer(new char[wStr.size() * 4]);  // 大小预留空间
	size_t out_len = wStr.size() * 4;
	iconv_t cd = iconv_open("ISO-8859-1", "UTF-16LE");
	if (cd == (iconv_t)-1) {
		throw std::runtime_error("Failed to open iconv descriptor.");
	}
	size_t in_len = wStr.size() * sizeof(wchar_t);
	const char* in = reinterpret_cast<const char*>(wStr.c_str());
	if (iconv(cd, const_cast<const char**>(&in), &in_len, reinterpret_cast<char**>(buffer.get()), &out_len) == (size_t)-1) {
		iconv_close(cd);
		throw std::runtime_error("Failed to convert wide string using iconv.");
	}
	iconv_close(cd);
	context.assign(buffer.get(), buffer.get() + (wStr.size() * 4 - out_len));
#else
	lgcr("stow: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)", lgm::er);
#endif

	return context;
}

std::string Typical_Tool::StringHandling::WstringToString(std::wstring&& wStr)
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
#elif _UNIX
	// POSIX 版本
	std::unique_ptr<char[]> buffer(new char[wStr.size() * 4]);  // 大小预留空间
	size_t out_len = wStr.size() * 4;
	iconv_t cd = iconv_open("ISO-8859-1", "UTF-16LE");
	if (cd == (iconv_t)-1) {
		throw std::runtime_error("Failed to open iconv descriptor.");
	}
	size_t in_len = wStr.size() * sizeof(wchar_t);
	const char* in = reinterpret_cast<const char*>(wStr.c_str());
	if (iconv(cd, const_cast<const char**>(&in), &in_len, reinterpret_cast<char**>(buffer.get()), &out_len) == (size_t)-1) {
		iconv_close(cd);
		throw std::runtime_error("Failed to convert wide string using iconv.");
	}
	iconv_close(cd);
	context.assign(buffer.get(), buffer.get() + (wStr.size() * 4 - out_len));
#else
	lgcr("stow: 转换失败, 没有声明对应平台(_WINDOWS/_UNIX)", lgm::er);
#endif

	return context;
}

float Typical_Tool::GameTools::GetFps()
{
	//临时统计
	static int tempCount = 0;
	static Timers timer; //时间
	static float fps; //帧率

	if (tempCount > FPS_COUNT)
	{
		//将时间节点拓展到两个
		if (timer.GetTimerSize() < 2)
			timer.AddTimer();

		tempCount = 0;
		timer.SetTimer(timer.GetTime(), 2);
		float tempTime = timer.计算首尾间隔();
		fps = FPS_COUNT / (tempTime / 1000.0f); //获取的系统时间为毫秒数
		timer.SetTimer(timer.GetTime(), 1);
	}

	tempCount++;
	return fps;
}
