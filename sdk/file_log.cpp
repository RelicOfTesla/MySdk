#include "stdafx.h"
#include "file_log.h"
#include "str_format.h"

void FileLog( const TCHAR* filepath, const char* s )
{
	HANDLE hFile = CreateFile( filepath, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, 0, 0 );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		SetFilePointer( hFile, 0, 0, FILE_END );
		DWORD bytes = 0;
		WriteFile( hFile, s, strlen( s ), &bytes, nullptr );
		WriteFile( hFile, "\r\n", 2, &bytes, nullptr );
	}
	CloseHandle( hFile );
}

void FileFormatLog( const TCHAR* filepath, const char* format, ... )
{
	va_list ap;
	va_start( ap, format );
	std::string s = str_vformat( format, ap );
	FileLog( filepath, s.c_str() );
}
