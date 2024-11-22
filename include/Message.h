#pragma once

#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "pch.h"
//#include "Typical_Tool.h"
#include "Log.h"


namespace Typical_Tool {


	namespace WindowsSystem {

		class  RegisterHotKeyMessage
		{
		private:
			Tstr 信息;

		public:
			RegisterHotKeyMessage(Tstr& RegisterHotKey热键信息, int message)
				: 信息(RegisterHotKey热键信息)
			{
				if (message > 0)
				{
					lgc("热键注册[" + this->信息 + "]: 成功👌");
					lgc();
				}
				else
				{
					lgc("热键注册[" + this->信息 + "]: 错误😒 -> 代码(" + Tto_string(message) + ")", lm::er);
					lgc();
				}
			}
			RegisterHotKeyMessage(Tstr&& RegisterHotKey热键信息, int message)
				: 信息(RegisterHotKey热键信息)
			{
				if (message > 0)
				{
					lgc("热键注册[" + this->信息 + "]: 成功👌");
					lgc();
				}
				else
				{
					lgc("热键注册[" + this->信息 + "]: 错误😒 -> 代码(" + Tto_string(message) + ")", lm::er);
					lgc();
				}
			}
		};
		typedef RegisterHotKeyMessage 热键注册消息;

		class  ShellMessage
		{
		private:
			Tstr 信息;
			int ErrorCode; //错误代码
			bool Status; //是否成功

		public:
			ShellMessage() {}
			ShellMessage(Tstr& Shell信息, int message)
				: 信息(Shell信息), ErrorCode(0), Status(false)
			{
				if (message < 32)
				{
					this->Status = false;

					//ShellExecute() 成功操作, 则传入为句柄
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 错误😒 -> 代码(" + Tto_string(message) + ")", lm::er);
					lgc();
				}
				else
				{
					this->Status = true;
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 成功 👌");
					lgc();
				}
			}
			ShellMessage(Tstr&& Shell信息, int message)
				: 信息(Shell信息), ErrorCode(0), Status(false)
			{
				if (message < 32)
				{
					this->Status = false;

					//ShellExecute() 成功操作, 则传入为句柄
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 错误😒 -> 代码(" + Tto_string(message) + ")", lm::er);
					lgc();
				}
				else
				{
					this->Status = true;
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 成功 👌");
					lgc();
				}
			}

			//错误代码
			int GetErrorCode();

			//是否成功
			bool IsSucceed();
		};
		typedef ShellMessage Shell消息;

	}
}

#endif