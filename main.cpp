#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#ifdef _DEBUG
#define _DEFAULT_FILENAME "C:\\Users\\User\\Downloads\\piggies.mp3"
#endif

#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <sstream>
#include <locale>
#include <codecvt>
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

int applyFix(fs::path original, fs::path newloc, const char *argv) {
	if (original == newloc) {
		size_t lastindex = original.string().find_last_of(".");
		std::string rawname = original.string().substr(0, lastindex);
		fs::rename(original, rawname + "-old.mp3");
		original = fs::path(rawname + "-old.mp3");
	}

	const fs::path ffmpegargv = fs::path(argv).parent_path() / "ffmpeg.exe";
	if (!fs::exists(ffmpegargv)) throw std::runtime_error("Could not find ffmpeg by path " + ffmpegargv.string());

	const std::string command = "\"" + ffmpegargv.string() + "\" -i \"" + original.string() + "\" -vn \"" + newloc.string() + "\"";

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring poo = converter.from_bytes(ffmpegargv.string());
	std::wstring args = converter.from_bytes(command);

	int res = startup(poo.c_str(), (LPWSTR)args.c_str());
	if (res == 0) {
		std::stringstream ss;
		ss << GetLastError();
		throw std::runtime_error("Windows error " + ss.str());
	}
	return res;
}

int main(int argc, const char** argv) {
	std::string filename;
	
#ifdef _DEFAULT_FILENAME
	filename = _DEFAULT_FILENAME;
#else
	if (argc > 1) {
		filename = argv[1];
	}
	else {
		std::cout << "Enter path to the .mp3 file: ";
		getline(std::cin, filename);
	}
#endif
	fs::path path = fs::path(filename);
	if (!fs::exists(path)) {
		std::cerr << "File was not found by given path: " << filename << std::endl;
		system("pause");
		return 1;
	}

	std::ifstream file(fs::absolute(path).string());
	unsigned char buffer[3];

	file.read((char*)&buffer[0], sizeof(buffer));
	file.close();

	std::string check(buffer, std::end(buffer));
	if (check == "ID3") {
		std::cout << "The .mp3 file is seemingly fine. If you believe this is wrong, enter \"continue\", to try to fix it anyway." << std::endl << std::endl << ">";
		std::string _inp;
		getline(std::cin, _inp);
		if (_inp != "continue") {
			return 0;
		}
	}
	else {
		std::cout << "Check failed, first three hex letters are not ID3" << std::endl;
	}

	try {
		applyFix(path, path, argv[0]);
		return 0;
	}
	catch (std::exception& e) {
		std::cout << "An error has occured: " << e.what() << std::endl;
		system("pause");
		return 1;
	}
}