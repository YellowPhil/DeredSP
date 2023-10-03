#define WIN32_NO_STATUS
#define SECURITY_WIN32
#include <windows.h>
#include <sspi.h>
#include <NTSecAPI.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <ntsecpkg.h>
#pragma comment(lib, "Secur32.lib")

void EnumSecPkg() {
    SECURITY_STATUS status;
    ULONG pcPackages = 0;
    SecPkgInfo* secPkgInfo = NULL;
    status = EnumerateSecurityPackages(&pcPackages, &secPkgInfo);
    if (status != SEC_E_OK) {
        wprintf(L"[!] EnumerateSecurityPackages() failed with error: %i\n", status);
    }
    std::wcout << L"NAME" << std::setw(50) << L"COMMENT" << std::setw(40) << L"VERSION" << std::endl;
    for (ULONG i = 0; i < pcPackages; i++) {
        std::wcout << secPkgInfo[i].Name << std::setw(75 - wcslen(secPkgInfo[i].Name)) << secPkgInfo[i].Comment << std::setw(20 - sizeof(secPkgInfo[i].wVersion)) << secPkgInfo[i].wVersion << std::endl;
    }
}

bool loadSecPackage(char path[]) {
    SECURITY_PACKAGE_OPTIONS spo = {};
    SECURITY_STATUS ss = AddSecurityPackageA(path, &spo);
    //ss = AddSecurityPackageA(packagePath2, &spo);
    if (ss != SEC_E_OK) {
        if (ss == SEC_E_SECPKG_NOT_FOUND) {
            std::wcout << L"[?] SEC_E_SECPKG_NOT_FOUND received! Check architecture. U should load x86 DLL into x86 system. x64 DLL into x64 systems" << std::endl;
            return 1;
        }
        else {
            std::wcout << L"[-] AddSecurityPackage failed: " << std::hex << ss << std::endl;
            return 1;
        }
    }
    else {
        std::wcout << L"[+] AddSecurityPackage Success" << std::endl;
        return 0;
    }

}

bool unloadSecPackage(char name[]) {
    SECURITY_STATUS ss = DeleteSecurityPackageA(name);
    //ss = AddSecurityPackageA(packagePath2, &spo);
    if (ss != SEC_E_OK) {
            std::wcout << L"[-] DeleteSecurityPackage failed: 0x" << std::hex << ss << std::endl;
            return 1;
    }
    else {
        std::wcout << L"[+] DeleteSecurityPackage Success" << std::endl;
        return 0;
    }
}

void toLower(char* str) {
    for (char* p = str; *p; ++p) {
        *p = *p > 0x40 && *p < 0x5b ? *p | 0x60 : *p;
    }
}


int main(int argc, char **argv)
{
    bool result{};
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " LOAD [path] /UNLOAD [name]"  <<  std::endl;
        return 1;
    }
    toLower(argv[1]);

    if (!strcmp(argv[1], "load"))
    {
        result = loadSecPackage(argv[2]);
    }
    else if (!strcmp(argv[1], "unload"))
    {
        result = unloadSecPackage(argv[2]);
    }
    else
    {
        std::cerr << "Usage: " << argv[0] << " LOAD [path] /UNLOAD [name]"  << std::endl;
        return 1;
    }
    EnumSecPkg();
    return result;
}