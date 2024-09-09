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

// 添加要在此处预编译的标头

#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>

#include <iomanip>
#include <sstream>
#include <locale>

#include <ctime>
#include <cstdio>
#include <cwchar>
#include <cstring>

#include <stdexcept>
#include <io.h>
#include <tchar.h>
#include <comdef.h>

#include <commctrl.h>

#ifdef _WINDOWS
#include <Windows.h>
#include <TlHelp32.h>
#elif _CONSOLE

#elif _UNIX
	//Linux 和 macOS
#include <iconv.h>
#endif

#endif
