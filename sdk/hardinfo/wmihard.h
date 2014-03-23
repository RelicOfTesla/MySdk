#pragma once

#include <list>
#include <string>

static const char*  wmikey_disk = "Win32_PhysicalMedia";
static const char*  wmikey_bios = "Win32_BIOS";
static const char*  wmikey_cpu = "Win32_Processor";
//static const char*  wmikey_mac = "Win32_PhysicalMedia";

std::list<std::string> get_wmi_serial(const std::string& key);


inline std::string GetDiskSerial()
{
	std::list<std::string> vlist = get_wmi_serial(wmikey_disk);
	if (vlist.size())
	{
		return *vlist.begin();
	}
	return "";
}
inline std::string GetBiosSerial()
{
	std::list<std::string> vlist = get_wmi_serial(wmikey_bios);
	if (vlist.size())
	{
		return *vlist.begin();
	}
	return "";
}

inline std::string GetCpuSerial()
{
	std::list<std::string> vlist = get_wmi_serial(wmikey_cpu);
	if (vlist.size())
	{
		return *vlist.begin();
	}
	return "";
}