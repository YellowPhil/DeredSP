#pragma once
#include <string>

namespace utils {
	std::string w2s(const std::wstring& w);
	unsigned int RSHash(const std::string& str);
	bool LoadSP(const std::string& path);
	std::vector<std::string> split(std::string s, std::string delimiter);
	std::string concat(const std::vector<std::string>& v, const std::string& c);
}
