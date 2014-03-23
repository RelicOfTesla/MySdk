#pragma once

#include <list>

const BYTE* CodeSearch(const void* pStart, size_t size, const char* pKey);
const BYTE* ModuleCodeSearch(HMODULE hLib, const char* pKey);
std::list<const BYTE*> ModuleCodeSearch_All(HMODULE hLib, const char* pKey);

void* CodeOffsetCalcPtr(const void* pCode, short off = 1);
__int32 CodeOffsetCalc32(const void* pCode, short off = 1);
__int64 CodeOffsetCalc64(const void* pCode, short off = 1);