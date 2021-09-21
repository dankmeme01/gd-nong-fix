#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>

#ifdef _DEBUG
#define _DEFAULT_FILENAME L"C:\\Users\\User\\Downloads\\piggies.mp3"
#endif

namespace fs = std::filesystem;

int startup(LPCWSTR lpApplicationName, LPWSTR args)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	int res = CreateProcess(lpApplicationName,   // the path
		args,           // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return res;
}

class nong_exception : public std::runtime_error {
public:
	nong_exception(const std::wstring& msg) : std::runtime_error("Error!"), message(msg) {};
	~nong_exception() throw() {};
	std::wstring msg() { return message; }
private:
	std::wstring message;
};