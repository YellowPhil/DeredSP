#include "Override.h"
#include <iostream>


bool Override::ReplaceImage(void *peData)
{
	BasicPE basicPE;
	if (!basicPE.ParseBuffer((BYTE *)peData)) return false;

	LPVOID newImg = ReplaceImage(peData, basicPE);
	if (newImg == nullptr) return false;

	InjectedCodeData iData;
	iData.imageBase = newImg;
	iData.pGetProcAddress = reinterpret_cast<f_GetProcAddress>(GetProcAddress);
	iData.pLoadLibraryA = reinterpret_cast<f_LoadLibraryA>(LoadLibraryA);
	//std::puts("\n\n[+] Parsing IAT and applying relocations [+]\n");
	InternalLoader(&iData);
	return true;
}


LPVOID Override::ReplaceImage(void *peData, BasicPE& pe)
{
	auto imgMem = VirtualAlloc(NULL, pe.pOptionalHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!imgMem) {
		return nullptr;
	}
	//std::printf("[+] Allocated memory at 0x%llx [+]\n[+] Copying PE header... [+]\n\n\n", (ULONG_PTR) imgMem);

	if (!memcpy(imgMem, peData, pe.pNtHeader->OptionalHeader.SizeOfHeaders))
	{
	//	std::printf("[-] Unable to copy PE header! [-]");
		VirtualFree(imgMem, 0, MEM_RELEASE);
		return nullptr;
	}
	//std::printf("[+] Writing sections! [+]\n\n");

	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pe.pNtHeader);

	// Write sections
	for (auto i = 0; i < pe.pFileHeader->NumberOfSections; ++i, ++pSectionHeader)
	{
		if (pSectionHeader->SizeOfRawData)
		{
			SIZE_T bytesWritten;
			if (!memcpy((BYTE*) imgMem + pSectionHeader->VirtualAddress, (char *)peData + pSectionHeader->PointerToRawData, pSectionHeader->SizeOfRawData))
			{
				//std::printf("Unable to write section into process memory! error => %d\n", GetLastError());
				VirtualFree(imgMem, 0, MEM_RELEASE);
				return nullptr;
			}
			//std::printf("\t[+] Written %s at 0x%llx [+]\n", pSectionHeader->Name, (ULONG_PTR)imgMem + pSectionHeader->VirtualAddress);
		}
	}

	//std::printf("\n[+] Done initial image mapping! [+]\n");
	return imgMem;

}
