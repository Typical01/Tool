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
			Ustr 信息;

		public:
			RegisterHotKeyMessage(Ustr& RegisterHotKey热键信息, int message)
				: 信息(RegisterHotKey热键信息)
			{
				if (message > 0)
				{
					lgc("热键注册[" + this->信息 + "]: 成功👌");
				}
				else
				{
					lgc("热键注册[" + this->信息 + "]: 错误😒 -> 代码(" + Uto_string(message) + ")", lgm::er);
				}
			}
			RegisterHotKeyMessage(Ustr&& RegisterHotKey热键信息, int message)
				: 信息(RegisterHotKey热键信息)
			{
				if (message > 0)
				{
					lgc("热键注册[" + this->信息 + "]: 成功👌");
				}
				else
				{
					lgc("热键注册[" + this->信息 + "]: 错误😒 -> 代码(" + Uto_string(message) + ")", lgm::er);
				}
			}
		};
		typedef RegisterHotKeyMessage 热键注册消息;

		class  ShellMessage
		{
		private:
			Ustr 信息;
			int ErrorCode; //错误代码
			bool Status; //是否成功

		public:
			ShellMessage() {}
			ShellMessage(Ustr& Shell信息, int message)
				: 信息(Shell信息), ErrorCode(0), Status(false)
			{
				if (message < 32)
				{
					this->Status = false;

					//ShellExecute() 成功操作, 则传入为句柄
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 错误😒 -> 代码(" + Uto_string(message) + ")", lgm::er);
				}
				else
				{
					this->Status = true;
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 成功 👌");
				}
			}
			ShellMessage(Ustr&& Shell信息, int message)
				: 信息(Shell信息), ErrorCode(0), Status(false)
			{
				if (message < 32)
				{
					this->Status = false;

					//ShellExecute() 成功操作, 则传入为句柄
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 错误😒 -> 代码(" + Uto_string(message) + ")", lgm::er);
				}
				else
				{
					this->Status = true;
					this->ErrorCode = message;
					lgc("Shell消息[" + this->信息 + "]: 成功 👌");
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