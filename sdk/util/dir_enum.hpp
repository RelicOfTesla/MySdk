#pragma once

#include <string>

inline static
void EnumDirectory(const TCHAR* dir_path, bool childs, const std::function<bool(const TCHAR*)>& pfn)
{
	std::basic_string<TCHAR> dir = dir_path;
	if (!dir.size())
	{
		return;
	}
	{
		TCHAR rch = *dir.rbegin();
		if (rch != TEXT('\\') && rch != TEXT('/'))
		{
			dir += TEXT('\\');
		}
	}

	WIN32_FIND_DATA FindInfo = {0};
	HANDLE hFind = FindFirstFile( (dir + TEXT("*")).c_str(), &FindInfo);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindInfo.cFileName[0] == TEXT('.') &&
					( FindInfo.cFileName[1] == TEXT('.') ||  FindInfo.cFileName[1] == 0 ) )
			{
				continue;
			}
			if (FindInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && childs)
			{
				EnumDirectory((dir + FindInfo.cFileName).c_str(), childs, pfn);
			}
			else if ((int)FindInfo.dwFileAttributes != -1)
			{
				if ( !pfn( (dir + FindInfo.cFileName).c_str() ) )
				{
					break;
				}
			}
		}
		while (FindNextFile(hFind, &FindInfo));
	}
	FindClose(hFind);
}
