#pragma once

#include "detours.h"
#include "dbg_assert.h"

#pragma comment(lib, "detours.lib")


inline void StartHook(IN OUT void** ppRawAddr, IN void* NewAddr)
{
	dbg_assert(*ppRawAddr);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(ppRawAddr, NewAddr);
	DetourTransactionCommit();
}

inline void StopHook(IN OUT void** ppRawAddr, IN void* NewAddr)
{
	dbg_assert(*ppRawAddr);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(ppRawAddr, NewAddr);
	DetourTransactionCommit();
}

