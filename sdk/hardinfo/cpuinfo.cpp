#include "stdafx.h"



struct CPUInfo
{
	std::string CPUVendor;//供货商
	std::string CPUH;//CPUID高位
	std::string CPUL;//CPUID低位
};

void GetCPUInfo(CPUInfo* Info)
{
	unsigned long s1, s2;
	unsigned char vendor_id[] = "------------"; //CPU提供商ID
	//CString str1,str2,str3;
	// 以下为获得CPU ID的汇编语言指令
	_asm    // 得到CPU提供商信息
	{
		xor eax, eax;  // 将eax清0
		cpuid;         // 获取CPUID的指令
		mov dword ptr vendor_id, ebx;
		mov dword ptr vendor_id[+4], edx;
		mov dword ptr vendor_id[+8], ecx;
	}
	Info->CPUVendor = (char*)vendor_id;

	_asm    // 得到CPU ID的高32位
	{
		mov eax, 01h;
		xor edx, edx;
		cpuid;
		mov s2, eax;
	}
	Info->CPUH = boost::lexical_cast<std::string>(s2) + "-";

	_asm    // 得到CPU ID的低64位
	{
		mov eax, 03h;
		xor ecx, ecx;
		xor edx, edx;
		cpuid;
		mov s1, edx;
		mov s2, ecx;
	}
	Info->CPUL = boost::lexical_cast<std::string>(s1) + "-" + boost::lexical_cast<std::string>(s2);
}

