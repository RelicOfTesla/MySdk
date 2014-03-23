#pragma once
#include "noncopyable.h"
#include "dbg_assert.h"

class CWinFile : noncopyable
{
public:
	CWinFile()
	{
		m_hFile = INVALID_HANDLE_VALUE;
	}
	CWinFile( const std::string& filepath, DWORD OpenType )
	{
		m_hFile = INVALID_HANDLE_VALUE;
		BOOL isopen = Open( filepath, OpenType );
		dbg_assert( isopen || OpenType == OPEN_EXISTING );
	}
	~CWinFile()
	{
		CloseHandle( m_hFile );
	}

	BOOL Open( const std::string& filepath, DWORD OpenType )
	{
		if( IsOpen() )
		{
			CloseHandle( m_hFile );
		}
		m_hFile = CreateFileA( filepath.c_str(), GENERIC_ALL, FILE_SHARE_READ, 0, OpenType, 0, 0 );
		return m_hFile != INVALID_HANDLE_VALUE;
	}

	BOOL IsOpen()const
	{
		return m_hFile != INVALID_HANDLE_VALUE;
	}
	BOOL IsEOF()const
	{
		return GetPos() == GetFileSize();
	}
	size_t GetPos()const
	{
		return ::SetFilePointer( m_hFile, 0, 0, FILE_CURRENT );
	}
	size_t Read( void* pv, size_t len )
	{
		dbg_assert( IsOpen() );
		DWORD bytes = 0;
		ReadFile( m_hFile, pv, len, &bytes, 0 );
		return bytes;
	}
	void Write( const void* pv, size_t len )
	{
		dbg_assert( IsOpen() );
		DWORD bytes = 0;
		WriteFile( m_hFile, pv, len, &bytes, 0 );
	}
	size_t GetFileSize()const
	{
		size_t n = ::GetFileSize( m_hFile, 0 );
		if( n == ( size_t ) - 1 )
		{
			n = 0;
		}
		return n;
	}
	void SetFilePointer( DWORD dwMoveMethod, DWORD lDistanceToMove )
	{
		::SetFilePointer( m_hFile, lDistanceToMove, 0, dwMoveMethod );
	}
protected:
	HANDLE m_hFile;
};

template<typename T> static bool LoadFileData( const std::string& filepath, T& data )
{
	CWinFile ifile;
	if( ifile.Open( filepath, OPEN_EXISTING ) )
	{
		data.resize( ifile.GetFileSize() );
		if( data.size() )
		{
			ifile.Read( &data[0], data.size() );
			return true;
		}
	}
	return false;
}
