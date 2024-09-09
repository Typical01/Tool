#pragma once

//Key.h 按键
#ifndef _KEY_H
#define _KEY_H
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#include "Typical_Tool.h"

#include "pch.h"

using namespace std;

namespace Typical_Tool {

	namespace WindowsSystem {
		//ContinuousClick 连续点击
		class  Key
		{
		private:
			//
		public:
			//模拟键盘按下 事件 (传入需要按的按键)
			static void keys(BYTE bVk);
			//获取键入状态
			static bool GetTypingStatus(int i);
			//获取键入状态
			static bool GetTypingStatus(int i, void(*func)());
			//按下 i 退出程序 c
			static void ExitProcedure(int i, const char* c);
			//模拟鼠标点击 事件(i:按下 i2:抬起 i3:间隔)
			static void keyJudge_Mouse(int i, int i2, int i3);
		};
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif