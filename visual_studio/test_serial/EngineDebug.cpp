#include "pch.h"
#include "EngineDebug.h"
#include <crtdbg.h>


namespace EngineDebug
{
	void LeakCheck()
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}

	void LeakPointBreak(int _Point)
	{
		_CrtSetBreakAlloc(_Point);
	}
}