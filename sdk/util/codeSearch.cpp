#include "stdafx.h"
#include "codeSearch.h"
#include <vector>
#include "dbg_assert.h"

namespace BinarySearch
{
	struct SearchRequest
	{
		int offset;
		std::vector<BYTE> data;
	};

	typedef std::list<SearchRequest> SearchRequestList;

	inline char _next_char(LPCSTR& pKey)
	{
		for(char ch = *pKey; ch; ++pKey)
		{
			if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' )
			{
				return ch;
			}
		}
		return 0;
	}

	bool InitRequest(const char* pKey, SearchRequestList& reqs)
	{
		reqs.clear();
		SearchRequest newNode;
		size_t cur_offset = 0;
		//	size_t skip_len = 0;
		char one[3];
		one[2] = 0;
		while(true)
		{
			one[0] = _next_char(pKey);
			if (one[0] == 0)
			{
				break;
			}
			one[1] = _next_char(pKey);
			if (one[1] == 0)
			{
				//throw std::logic_error("error sign length");
				return false;
			}
			if(one[0] == '?')
			{
				if(one[1] != '?')
				{
					//throw std::logic_error("error sign");
					return false;
				}
				if(newNode.data.size())
				{
					newNode.offset = cur_offset - newNode.data.size();
					reqs.push_back(newNode);
					newNode.data.clear();
					//				skip_len = 0;
				}
				++cur_offset;
				//			++skip_len;
				continue;
			}
			long val = strtol(one, 0, 16);
			newNode.data.push_back( BYTE(val) );
			++cur_offset;
		}
		if(newNode.data.size())
		{
			newNode.offset = cur_offset - newNode.data.size();
			reqs.push_back(newNode);
		}
		return true;
	};

	const BYTE* Search(const void* pStart, size_t size, const SearchRequestList& reqs)
	{
		if (reqs.size() == 0)
		{
			return nullptr;
		}
		size_t total_size = reqs.back().offset + reqs.back().data.size();
		if(size < total_size || total_size == 0)
		{
			return 0;
		}
		const BYTE* pData = (const BYTE*)pStart;
		for(size_t i = 0; i < size - total_size; ++i)
		{
			const BYTE* pCur = &pData[i];
			bool error = false;
			for(SearchRequestList::const_iterator it = reqs.begin(); it != reqs.end(); ++it)
			{
				const SearchRequestList::value_type& v = *it;
				if(memcmp(pCur + v.offset, &v.data[0], v.data.size()) != 0)
				{
					error = true;
					break;
				}
			}
			if(!error)
			{
				return pCur;
			}
		}
		return 0;
	}

};


const BYTE* CodeSearch(const void* pStart, size_t size, const char* pKey)
{
	BinarySearch::SearchRequestList reqs;
	BinarySearch::InitRequest(pKey, reqs);
	return BinarySearch::Search(pStart, size, reqs);
}
const BYTE* CodeSearch(const void* pStart, const char* pKey)
{
	MEMORY_BASIC_INFORMATION mbi = {0};
	VirtualQuery(pStart, &mbi, sizeof(mbi));
	size_t len = mbi.RegionSize - ( PBYTE(pStart) - PBYTE(mbi.AllocationBase) );
	return CodeSearch(pStart, len, pKey);
}

const BYTE* ModuleCodeSearch(HMODULE hLib, const char* pKey)
{
	BinarySearch::SearchRequestList reqs;
	BinarySearch::InitRequest(pKey, reqs);

	dbg_assert(hLib);
	IMAGE_DOS_HEADER* pDos = (IMAGE_DOS_HEADER*)hLib;
	IMAGE_NT_HEADERS* pNT32 = (IMAGE_NT_HEADERS*)(PBYTE(hLib) + pDos->e_lfanew);
	size_t hdrsize = sizeof(IMAGE_NT_HEADERS) - sizeof(IMAGE_OPTIONAL_HEADER) + pNT32->FileHeader.SizeOfOptionalHeader;
	IMAGE_SECTION_HEADER* pSec = (IMAGE_SECTION_HEADER*)(PBYTE(pNT32) + hdrsize);
	for(int i = 0; i < pNT32->FileHeader.NumberOfSections; ++i)
	{
		if( pSec[i].Characteristics & IMAGE_SCN_MEM_EXECUTE )
		{
			DWORD VAD32 = pSec[i].VirtualAddress;
			if (VAD32 < (DWORD)hLib)
			{
				VAD32 += (DWORD)hLib;
			}
			const BYTE* r = BinarySearch::Search( (void*)VAD32, pSec[i].SizeOfRawData, reqs );
			if (r)
			{
				return r;
			}
		}
	}
	return nullptr;
}


std::list<const BYTE*> ModuleCodeSearch_All(HMODULE hLib, const char* pKey)
{
	std::list<const BYTE*> result;
	dbg_assert(hLib);

	BinarySearch::SearchRequestList reqs;
	BinarySearch::InitRequest(pKey, reqs);

	IMAGE_DOS_HEADER* pDos = (IMAGE_DOS_HEADER*)hLib;
	IMAGE_NT_HEADERS* pNT32 = (IMAGE_NT_HEADERS*)(PBYTE(hLib) + pDos->e_lfanew);
	size_t hdrsize = sizeof(IMAGE_NT_HEADERS) - sizeof(IMAGE_OPTIONAL_HEADER) + pNT32->FileHeader.SizeOfOptionalHeader;
	IMAGE_SECTION_HEADER* pSec = (IMAGE_SECTION_HEADER*)(PBYTE(pNT32) + hdrsize);
	for(int i = 0; i < pNT32->FileHeader.NumberOfSections; ++i)
	{
		if( pSec[i].Characteristics & IMAGE_SCN_MEM_EXECUTE )
		{
			DWORD VAD32 = pSec[i].VirtualAddress;
			if (VAD32 < (DWORD)hLib)
			{
				VAD32 += (DWORD)hLib;
			}
			const BYTE* r = BinarySearch::Search( (void*)VAD32, pSec[i].SizeOfRawData, reqs );
			if (r)
			{
				result.push_back(r);
			}
		}
	}
	return result;
}

void* CodeOffsetCalcPtr(const void* pCode, short off)
{
	const BYTE* pData = (const BYTE*)pCode;
	void* val = *(void**)&pData[off];
	return (void*)(PBYTE(pData) + ptrdiff_t(val));
}
__int32 CodeOffsetCalc32(const void* pCode, short off)
{
	const BYTE* pData = (const BYTE*)pCode;
	__int32 val = *(__int32*)&pData[off];
	return (__int32)(pData + val);
}
__int64 CodeOffsetCalc64(const void* pCode, short off)
{
	const BYTE* pData = (const BYTE*)pCode;
	__int64 val = *(__int64*)&pData[off];
	return (__int64)(pData + val);
}