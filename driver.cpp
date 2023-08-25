#include "driver.h"

uintptr_t ProcId;
uintptr_t BaseId;
HANDLE driver_handle;

bool driver::find_driver()
{
	driver_handle = CreateFileW((L"\\\\.\\\pymodule"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
		return false;

	return true;
}

void driver::read_virtual_memory(PVOID address, PVOID buffer, DWORD size)
{
	t_virtual arguments = { 0 };

	arguments.security_code = CODE_SECURITY;
	arguments.address = (ULONGLONG)address;
	arguments.buffer = (ULONGLONG)buffer;
	arguments.size = size;
	arguments.process_id = ProcId;
	arguments.virtual_mode = VIRTUAL_READ;

	DeviceIoControl(driver_handle, CODE_VIRTUAL, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

void driver::write_virtual_memory(PVOID address, PVOID buffer, DWORD size)
{
	t_virtual arguments = { 0 };

	arguments.security_code = CODE_SECURITY;
	arguments.address = (ULONGLONG)address;
	arguments.buffer = (ULONGLONG)buffer;
	arguments.size = size;
	arguments.process_id = ProcId;
	arguments.virtual_mode = VIRTUAL_WRITE;

	DeviceIoControl(driver_handle, CODE_VIRTUAL, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

void driver::read_physical_memory(PVOID address, PVOID buffer, DWORD size)
{
	t_virtual arguments = { 0 };

	arguments.security_code = CODE_SECURITY;
	arguments.address = (ULONGLONG)address;
	arguments.buffer = (ULONGLONG)buffer;
	arguments.size = size;
	arguments.process_id = ProcId;
	arguments.virtual_mode = PHYSICAL_READ;

	DeviceIoControl(driver_handle, CODE_PHYSICAL, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

bool driver::read_raw(uint64_t address, void* buffer, size_t size)
{
	driver::read_physical_memory((PVOID)address, buffer, size);
	return true;
}

void driver::write_physical_memory(PVOID address, PVOID buffer, DWORD size)
{
	t_virtual arguments = { 0 };

	arguments.security_code = CODE_SECURITY;
	arguments.address = (ULONGLONG)address;
	arguments.buffer = (ULONGLONG)buffer;
	arguments.size = size;
	arguments.process_id = ProcId;
	arguments.virtual_mode = PHYSICAL_WRITE;

	DeviceIoControl(driver_handle, CODE_PHYSICAL, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

uintptr_t driver::find_image()
{
	uintptr_t image_address = { NULL };
	t_image arguments = { NULL };

	arguments.security_code = CODE_SECURITY;
	arguments.process_id = ProcId;
	arguments.address = (ULONGLONG*)&image_address;

	DeviceIoControl(driver_handle, CODE_IMAGE, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

	return image_address;
}

INT32 driver::find_process(LPCTSTR process_name)
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, process_name)) {
				CloseHandle(hsnap);
				ProcId = pt.th32ProcessID;
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);
	return { NULL };
}
