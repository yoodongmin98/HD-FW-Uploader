#pragma once
#include <Windows.h>
#include <assert.h>
#include <string>

// 설명 : Debug용 기본 namespace - 주로 LeakCheck
namespace EngineDebug
{
	void LeakCheck();
	void LeakPointBreak(int _Point);
};


#define MsgAssert(MsgText) std::string ErrorText = MsgText; MessageBoxA(nullptr, ErrorText.c_str(), "Error", MB_OK); assert(false);

#define MsgTextBox(MsgText) std::string ErrorText = MsgText; MessageBoxA(nullptr, ErrorText.c_str(), "Text", MB_OK);