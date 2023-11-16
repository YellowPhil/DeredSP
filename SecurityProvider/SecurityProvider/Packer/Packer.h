#pragma once
#include <string>
#include <windows.h>
#include <PEFILE.h>

class Packer final
{
public:
	Packer() = default;
	void Pack(const std::string& inputFile, const std::string& fileToPack, const std::string& outputFilePath);
	~Packer() = default;
};
