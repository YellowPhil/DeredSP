#include <string>
#include <windows.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <cstdint>

#include "Packer.h"


void Packer::Pack(const std::string& inputFilePath, const std::string& targetFilePath, const std::string& outputFilePath) {
	uintmax_t target_size, input_size;
	std::ifstream inputFile, targetFile;
	std::ofstream outputFile;

	inputFile.open(inputFilePath, std::ios::in | std::ios::binary);
	targetFile.open(targetFilePath, std::ios::in | std::ios::binary);

	if (inputFile.fail() || targetFile.fail()) {
		throw std::invalid_argument("Could not open input or target file");
	}

	inputFile >> std::noskipws;
	targetFile >> std::noskipws;

	outputFile.open(outputFilePath, std::ios::out | std::ios::binary);
	if (outputFile.fail()) {
		throw std::invalid_argument("Could not open output file for writing");
	}

	outputFile << std::noskipws;

/*
	try {
		target_size = std::filesystem::file_size(targetFilePath);
		input_size = std::filesystem::file_size(inputFilePath);
	}
	catch (std::filesystem::filesystem_error& e) {
		throw std::invalid_argument("Could not determine size of input and target files on disk");
	}
*/

	std::vector<uint8_t> targetFileBuf{
		std::istream_iterator<uint8_t>(targetFile),
		std::istream_iterator<uint8_t>()
	
	};

	std::vector<uint8_t> inputFileBuf {
		std::istream_iterator<uint8_t>(inputFile),
		std::istream_iterator<uint8_t>()
	
	};

	auto DOSHeader = reinterpret_cast<__IMAGE_DOS_HEADER *>(inputFileBuf.data());
	char emptyRichHeader[0x400] = { 0 };
	DOSHeader->e_lfanew += 0x400;
	DOSHeader->e_lfanew += targetFileBuf.size();

	outputFile.write(reinterpret_cast<char*>(DOSHeader), sizeof(__IMAGE_DOS_HEADER));;
	outputFile.write(emptyRichHeader, sizeof(emptyRichHeader));
	outputFile.write(reinterpret_cast<char*>(targetFileBuf.data()), targetFileBuf.size());
	outputFile.write(reinterpret_cast<char*>(&inputFileBuf[sizeof(__IMAGE_DOS_HEADER)]), inputFileBuf.size() - sizeof(__IMAGE_DOS_HEADER));
	/*
		 
	targetFile.read((char*)targetFileBuf.get(), target_size);
	inputFile.read((char*)inputFileBuf.get(), input_size);

	if (inputFile.fail() && targetFile.fail()) {
		throw std::runtime_error("Could not read from input or targetFile");
	}

	DOSHeader->e_lfanew += sizeof(emptyRichHeader);
	DOSHeader->e_lfanew += target_size;

	outputFile.write((char*)DOSHeader.get(), sizeof(__IMAGE_DOS_HEADER));
	outputFile.write(emptyRichHeader, sizeof(emptyRichHeader));
	outputFile.write((char*)targetFileBuf.get(), target_size);
	outputFile.write((char*)inputFileBuf.get(), input_size - sizeof(__IMAGE_DOS_HEADER));

	
	auto target_read = fread(targetFileBuf.get(), sizeof(uint8_t), target_size, tfile);
	auto input_read = fread(inputFileBuf.get(), sizeof(uint8_t), input_size, tfile);
	
	if (target_read != target_size || input_read != input_size) {
		throw std::runtime_error("Coult not  read input or target file");
	}

	input_elfanew += target_size;

	fwrite(&DOSHeader, sizeof(IMAGE_DOS_HEADER) - sizeof(DOSHeader.e_lfanew), 1, ofile);
	fwrite(&input_elfanew, sizeof(input_elfanew), 1, ofile);
	fwrite(targetFileBuf.get(), sizeof(uint8_t), target_size, ofile);

	fseek(ifile, sizeof(IMAGE_DOS_HEADER), SEEK_SET);
	try {
	fwrite(inputFileBuf.get(), sizeof(uint8_t), input_size - sizeof(IMAGE_DOS_HEADER), ofile);
	*/
}
