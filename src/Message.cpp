#include "pch.h"
#include "Message.h"


int Typical_Tool::WindowsSystem::ShellMessage::GetErrorCode()
{
	return this->ErrorCode;
}

bool Typical_Tool::WindowsSystem::ShellMessage::IsSucceed()
{
	return this->Status;
}