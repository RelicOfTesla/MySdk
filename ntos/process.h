#pragma once


enum
{
	SE_BACKUP_PRIVILEGE = 17,
	SE_RESTORE_PRIVILEGE = 18,
	SE_SHUTDOWN_PRIVILEGE = 19,
	SE_DEBUG_PRIVILEGE = 20
};

typedef NTSTATUS( NTAPI* LPFN_RtlAdjustPrivilege )(
	ULONG Privilege,
	BOOL Enable,
	BOOL CurrentThread,
	BOOL* Enabled
);

enum THREAD_INFORMATION_CLASS
{
	ThreadBasicInformation, ThreadTimes, ThreadPriority,
	ThreadBasePriority, ThreadAffinityMask, ThreadImpersonationToken,
	ThreadDescriptorTableEntry, ThreadEnableAlignmentFaultFixup, ThreadEventPair,
	ThreadQuerySetWin32StartAddress, ThreadZeroTlsCell, ThreadPerformanceCount,
	ThreadAmILastThread, ThreadIdealProcessor, ThreadPriorityBoost,
	ThreadSetTlsArrayAddress, ThreadIsIoPending, ThreadHideFromDebugger,
};
typedef NTSTATUS( NTAPI* LPFN_NtSetInformationThread )(
	IN HANDLE ThreadHandle,
	IN THREAD_INFORMATION_CLASS ThreadInformationClass,
	IN PVOID ThreadInformation,
	IN ULONG ThreadInformationLength
);

enum PROCESS_INFORMATION_CLASS
{
	ProcessBasicInformation, ProcessQuotaLimits, ProcessIoCounters,
	ProcessVmCounters, ProcessTimes, ProcessBasePriority,
	ProcessRaisePriority, ProcessDebugPort, ProcessExceptionPort,
	ProcessAccessToken, ProcessLdtInformation, ProcessLdtSize,
	ProcessDefaultHardErrorMode, ProcessIoPortHandlers, ProcessPooledUsageAndLimits,
	ProcessWorkingSetWatch, ProcessUserModeIOPL, ProcessEnableAlignmentFaultFixup,
	ProcessPriorityClass, ProcessWx86Information, ProcessHandleCount,
	ProcessAffinityMask, ProcessPriorityBoost, MaxProcessInfoClass
};
typedef NTSTATUS( NTAPI* LPFN_NtSetInformationProcess )(
	IN HANDLE ProcessHandle,
	IN PROCESS_INFORMATION_CLASS ProcessInformationClass,
	IN PVOID ProcessInformation,
	IN ULONG ProcessInformationLength
);
