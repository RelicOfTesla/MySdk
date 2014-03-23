#include "stdafx.h"



struct CPUInfo
{
	std::string CPUVendor;//������
	std::string CPUH;//CPUID��λ
	std::string CPUL;//CPUID��λ
};

void GetCPUInfo(CPUInfo* Info)
{
	unsigned long s1, s2;
	unsigned char vendor_id[] = "------------"; //CPU�ṩ��ID
	//CString str1,str2,str3;
	// ����Ϊ���CPU ID�Ļ������ָ��
	_asm    // �õ�CPU�ṩ����Ϣ
	{
		xor eax, eax;  // ��eax��0
		cpuid;         // ��ȡCPUID��ָ��
		mov dword ptr vendor_id, ebx;
		mov dword ptr vendor_id[+4], edx;
		mov dword ptr vendor_id[+8], ecx;
	}
	Info->CPUVendor = (char*)vendor_id;

	_asm    // �õ�CPU ID�ĸ�32λ
	{
		mov eax, 01h;
		xor edx, edx;
		cpuid;
		mov s2, eax;
	}
	Info->CPUH = boost::lexical_cast<std::string>(s2) + "-";

	_asm    // �õ�CPU ID�ĵ�64λ
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

