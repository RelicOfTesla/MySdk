#pragma once

#include <crtdbg.h>

#if _DEBUG

#define dbg_assert(x) if(!(x)){\
	if(IsDebuggerPresent()) \
		_CrtDbgBreak();\
	else if( _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, _CRT_WIDE(#x)) == 1 )\
		_CrtDbgBreak();\
 }


#else
#define	dbg_assert(x)
#endif

