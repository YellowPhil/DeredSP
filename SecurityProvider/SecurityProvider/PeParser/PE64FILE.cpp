#include "PE64FILE.h"

PE64FILE::PE64FILE(FILE* _Ppefile) {
	Ppefile = _Ppefile;
	ParseFile();

}

// ADDRESS RESOLVERS
int PE64FILE::locate(DWORD VA) {
	int index;
	for (int i = 0; i < PEFILE_NT_HEADERS_FILE_HEADER_NUMBER0F_SECTIONS; i++) {
		if (VA >= PEFILE_SECTION_HEADERS[i].VirtualAddress
			&& VA < (PEFILE_SECTION_HEADERS[i].VirtualAddress + PEFILE_SECTION_HEADERS[i].Misc.VirtualSize)){
			index = i;
			break;
		}
	}
	return index;
}

DWORD PE64FILE::resolve(DWORD VA, int index) {
	return (VA - PEFILE_SECTION_HEADERS[index].VirtualAddress) + PEFILE_SECTION_HEADERS[index].PointerToRawData;

}

// PARSERS
void PE64FILE::ParseDOSHeader() {
	fseek(Ppefile, 0, SEEK_SET);
	fread(&PEFILE_DOS_HEADER, sizeof(___IMAGE_DOS_HEADER), 1, Ppefile);

	PEFILE_DOS_HEADER_EMAGIC = PEFILE_DOS_HEADER.e_magic;
	PEFILE_DOS_HEADER_LFANEW = PEFILE_DOS_HEADER.e_lfanew;

}

void PE64FILE::ParseNTHeaders() {
	
	fseek(Ppefile, PEFILE_DOS_HEADER.e_lfanew, SEEK_SET);
	fread(&PEFILE_NT_HEADERS, sizeof(PEFILE_NT_HEADERS), 1, Ppefile);

	PEFILE_NT_HEADERS_SIGNATURE = PEFILE_NT_HEADERS.Signature;

	PEFILE_NT_HEADERS_FILE_HEADER_MACHINE = PEFILE_NT_HEADERS.FileHeader.Machine;
	PEFILE_NT_HEADERS_FILE_HEADER_NUMBER0F_SECTIONS = PEFILE_NT_HEADERS.FileHeader.NumberOfSections;
	PEFILE_NT_HEADERS_FILE_HEADER_SIZEOF_OPTIONAL_HEADER = PEFILE_NT_HEADERS.FileHeader.SizeOfOptionalHeader;

	PEFILE_NT_HEADERS_OPTIONAL_HEADER_MAGIC = PEFILE_NT_HEADERS.OptionalHeader.Magic;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_SIZEOF_CODE = PEFILE_NT_HEADERS.OptionalHeader.SizeOfCode;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_SIZEOF_INITIALIZED_DATA = PEFILE_NT_HEADERS.OptionalHeader.SizeOfInitializedData;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_SIZEOF_UNINITIALIZED_DATA = PEFILE_NT_HEADERS.OptionalHeader.SizeOfUninitializedData;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_ADDRESSOF_ENTRYPOINT = PEFILE_NT_HEADERS.OptionalHeader.AddressOfEntryPoint;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_BASEOF_CODE = PEFILE_NT_HEADERS.OptionalHeader.BaseOfCode;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_IMAGEBASE = PEFILE_NT_HEADERS.OptionalHeader.ImageBase;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_SECTION_ALIGNMENT = PEFILE_NT_HEADERS.OptionalHeader.SectionAlignment;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_FILE_ALIGNMENT = PEFILE_NT_HEADERS.OptionalHeader.FileAlignment;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_SIZEOF_IMAGE = PEFILE_NT_HEADERS.OptionalHeader.SizeOfImage;
	PEFILE_NT_HEADERS_OPTIONAL_HEADER_SIZEOF_HEADERS = PEFILE_NT_HEADERS.OptionalHeader.SizeOfHeaders;

	PEFILE_EXPORT_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_EXPORT];
	PEFILE_IMPORT_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_IMPORT];
	PEFILE_RESOURCE_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_RESOURCE];
	PEFILE_EXCEPTION_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_EXCEPTION];
	PEFILE_SECURITY_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_SECURITY];
	PEFILE_BASERELOC_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_BASERELOC];
	PEFILE_DEBUG_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_DEBUG];
	PEFILE_ARCHITECTURE_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_ARCHITECTURE];
	PEFILE_GLOBALPTR_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_GLOBALPTR];
	PEFILE_TLS_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_TLS];
	PEFILE_LOAD_CONFIG_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
	PEFILE_BOUND_IMPORT_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT];
	PEFILE_IAT_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_IAT];
	PEFILE_DELAY_IMPORT_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];
	PEFILE_COM_DESCRIPTOR_DIRECTORY = PEFILE_NT_HEADERS.OptionalHeader.DataDirectory[___IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR];

}

void PE64FILE::ParseSectionHeaders() {
	
	PEFILE_SECTION_HEADERS = new ___IMAGE_SECTION_HEADER[PEFILE_NT_HEADERS_FILE_HEADER_NUMBER0F_SECTIONS];
	for (int i = 0; i < PEFILE_NT_HEADERS_FILE_HEADER_NUMBER0F_SECTIONS; i++) {
		int offset = (PEFILE_DOS_HEADER.e_lfanew + sizeof(PEFILE_NT_HEADERS)) + (i * ___IMAGE_SIZEOF_SECTION_HEADER);
		fseek(Ppefile, offset, SEEK_SET);
		fread(&PEFILE_SECTION_HEADERS[i], ___IMAGE_SIZEOF_SECTION_HEADER, 1, Ppefile);
	}

}

void PE64FILE::ParseImportDirectory() {
	
	DWORD _import_directory_address = resolve(PEFILE_IMPORT_DIRECTORY.VirtualAddress, locate(PEFILE_IMPORT_DIRECTORY.VirtualAddress));
	_import_directory_count = 0;

	while (true) {
		___IMAGE_IMPORT_DESCRIPTOR tmp;
		int offset = (_import_directory_count * sizeof(___IMAGE_IMPORT_DESCRIPTOR)) + _import_directory_address;
		fseek(Ppefile, offset, SEEK_SET);
		fread(&tmp, sizeof(___IMAGE_IMPORT_DESCRIPTOR), 1, Ppefile);

		if (tmp.Name == 0x00000000 && tmp.FirstThunk == 0x00000000) {
			_import_directory_count -= 1;
			_import_directory_size = _import_directory_count * sizeof(___IMAGE_IMPORT_DESCRIPTOR);
			break;
		}

		_import_directory_count++;
	}

	PEFILE_IMPORT_TABLE = new ___IMAGE_IMPORT_DESCRIPTOR[_import_directory_count];

	for (int i = 0; i < _import_directory_count; i++) {
		int offset = (i * sizeof(___IMAGE_IMPORT_DESCRIPTOR)) + _import_directory_address;
		fseek(Ppefile, offset, SEEK_SET);
		fread(&PEFILE_IMPORT_TABLE[i], sizeof(___IMAGE_IMPORT_DESCRIPTOR), 1, Ppefile);
	}

}

void PE64FILE::ParseBaseReloc() {
	
	DWORD _basereloc_directory_address = resolve(PEFILE_BASERELOC_DIRECTORY.VirtualAddress, locate(PEFILE_BASERELOC_DIRECTORY.VirtualAddress));
	_basreloc_directory_count = 0;
	int _basereloc_size_counter = 0;

	while (true) {
		___IMAGE_BASE_RELOCATION tmp;

		int offset = (_basereloc_size_counter + _basereloc_directory_address);

		fseek(Ppefile, offset, SEEK_SET);
		fread(&tmp, sizeof(___IMAGE_BASE_RELOCATION), 1, Ppefile);

		if (tmp.VirtualAddress == 0x00000000 &&
			tmp.SizeOfBlock == 0x00000000) {
			break;
		}

		_basreloc_directory_count++;
		_basereloc_size_counter += tmp.SizeOfBlock;
	}

	PEFILE_BASERELOC_TABLE = new ___IMAGE_BASE_RELOCATION[_basreloc_directory_count];

	_basereloc_size_counter = 0;

	for (int i = 0; i < _basreloc_directory_count; i++) {
		int offset = _basereloc_directory_address + _basereloc_size_counter;
		fseek(Ppefile, offset, SEEK_SET);
		fread(&PEFILE_BASERELOC_TABLE[i], sizeof(___IMAGE_BASE_RELOCATION), 1, Ppefile);
		_basereloc_size_counter += PEFILE_BASERELOC_TABLE[i].SizeOfBlock;
	}

}

void PE64FILE::ParseRichHeader() {
	
	char* dataPtr = new char[PEFILE_DOS_HEADER_LFANEW];
	fseek(Ppefile, 0, SEEK_SET);
	fread(dataPtr, PEFILE_DOS_HEADER_LFANEW, 1, Ppefile);

	int index_ = 0;

	for (int i = 0; i <= PEFILE_DOS_HEADER_LFANEW; i++) {
		if (dataPtr[i] == 0x52 && dataPtr[i + 1] == 0x69) {
			index_ = i;
			break;
		}
	}

	if (index_ == 0) {
		printf("Error while parsing Rich Header.");
		PEFILE_RICH_HEADER_INFO.entries = 0;
		return;
	}

	char key[4];
	memcpy(key, dataPtr + (index_ + 4), 4);

	int indexpointer = index_ - 4;
	int RichHeaderSize = 0;

	while (true) {
		char tmpchar[4];
		memcpy(tmpchar, dataPtr + indexpointer, 4);

		for (int i = 0; i < 4; i++) {
			tmpchar[i] = tmpchar[i] ^ key[i];
		}

		indexpointer -= 4;
		RichHeaderSize += 4;

		if (tmpchar[1] = 0x61 && tmpchar[0] == 0x44) {
			break;
		}
	}

	char* RichHeaderPtr = new char[RichHeaderSize];
	memcpy(RichHeaderPtr, dataPtr + (index_ - RichHeaderSize), RichHeaderSize);

	for (int i = 0; i < RichHeaderSize; i += 4) {

		for (int x = 0; x < 4; x++) {
			RichHeaderPtr[i + x] = RichHeaderPtr[i + x] ^ key[x];
		}

	}

	PEFILE_RICH_HEADER_INFO.size = RichHeaderSize;
	PEFILE_RICH_HEADER_INFO.ptrToBuffer = RichHeaderPtr;
	PEFILE_RICH_HEADER_INFO.entries = (RichHeaderSize - 16) / 8;

	delete[] dataPtr;

	PEFILE_RICH_HEADER.entries = new RICH_HEADER_ENTRY[PEFILE_RICH_HEADER_INFO.entries];

	for (int i = 16; i < RichHeaderSize; i += 8) {
		WORD PRODID = (uint16_t)((unsigned char)RichHeaderPtr[i + 3] << 8) | (unsigned char)RichHeaderPtr[i + 2];
		WORD BUILDID = (uint16_t)((unsigned char)RichHeaderPtr[i + 1] << 8) | (unsigned char)RichHeaderPtr[i];
		DWORD USECOUNT = (uint32_t)((unsigned char)RichHeaderPtr[i + 7] << 24) | (unsigned char)RichHeaderPtr[i + 6] << 16 | (unsigned char)RichHeaderPtr[i + 5] << 8 | (unsigned char)RichHeaderPtr[i + 4];
		PEFILE_RICH_HEADER.entries[(i / 8) - 2] = {
			PRODID,
			BUILDID,
			USECOUNT
		};
		if (i + 8 >= RichHeaderSize) {
			PEFILE_RICH_HEADER.entries[(i / 8) - 1] = { 0x0000, 0x0000, 0x00000000 };
		}
	}
	delete[] PEFILE_RICH_HEADER_INFO.ptrToBuffer;
}

void PE64FILE::ParseFile() {
	ParseDOSHeader();
	ParseRichHeader();
	ParseNTHeaders();
	ParseSectionHeaders();
	ParseImportDirectory();
	ParseBaseReloc();
}
