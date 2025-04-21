// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

//非 CMake
//#ifdef _DEBUG
//#pragma comment(lib, "../x64/Debug/libTools_Log.lib")
//#else
//#pragma comment(lib, "../x64/Release/libTools_Log.lib")
//#endif

#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <future>
#include <type_traits>

#include <cassert>

#include <iomanip>
#include <sstream>

#include <locale>
#include <codecvt>

#include <ctime>
#include <cstdio>
#include <cwchar>
#include <cstring>

#include <stdexcept>


#ifdef _WINDOWS
#define WINVER 0x0A00 // Windows 10
#define _WIN32_WINNT 0x0A00 // Windows 10




#include <io.h>
#include <comdef.h>
#include <commctrl.h>
#include <tchar.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <ShellScalingAPI.h>

#ifdef WIN32APP
#pragma comment(lib, "Shcore.lib")
#include <Shcore.h>
#endif
#endif








#endif