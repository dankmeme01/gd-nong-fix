#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "util.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <string>
#include <locale>

int applyFix(fs::path original, fs::path newloc, const wchar_t *argv) {
	if (original == newloc) {
		size_t lastindex = original.string().find_last_of(".");
		std::string rawname = original.string().substr(0, lastindex);
		fs::rename(original, rawname + "-old.mp3");
		original = fs::path(rawname + "-old.mp3");
	}

	const fs::path ffmpegargv = fs::path(argv).parent_path() / "ffmpeg.exe";
	if (!fs::exists(ffmpegargv)) throw nong_exception(L"Could not find ffmpeg by path " + ffmpegargv.wstring());

	const std::wstring args = L"\"" + ffmpegargv.wstring() + L"\" -i \"" + original.wstring() + L"\" -vn \"" + newloc.wstring() + L"\"";

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring poo = converter.from_bytes(ffmpegargv.string());
	//std::wstring args = converter.from_bytes(command);

	int res = startup(poo.c_str(), (LPWSTR)args.c_str());
	if (res == 0) {
		std::stringstream ss;
		ss << GetLastError();
		throw std::runtime_error("Windows error " + ss.str());
	}
	return res;
}

int wmain(int argc, const wchar_t** argv) {
	std::wstring filename;
	
#ifdef _DEFAULT_FILENAME
	filename = _DEFAULT_FILENAME;
#else
	if (argc > 1) {
		filename = argv[1];
	}
	else {
		std::cout << "Enter path to the .mp3 file: ";
		getline(std::wcin, filename);
		if (filename.at(0) == L'"' && filename.at(filename.size()-1) == L'"') {
			filename = filename.substr(1, filename.size()-2);
		}
	}
#endif
	fs::path path = fs::absolute(fs::path(filename));
	if (!fs::exists(path)) {
		std::wcerr << L"File was not found by given path: " << path.wstring() << std::endl;
		system("pause");
		return 1;
	}

	std::ifstream file(path);
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
	catch (nong_exception& e) {
		std::wcout << L"An error has occured: " << e.msg() << std::endl;
		system("pause");
		return 1;
	}
	catch (std::exception& e) {
		std::cout << "An error has occured: " << e.what() << std::endl;
		system("pause");
		return 1;
	}
}