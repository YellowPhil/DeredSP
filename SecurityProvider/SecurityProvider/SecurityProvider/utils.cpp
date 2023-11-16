#define SECURITY_WIN32
#define WIN32_NO_STATUS
#include <windows.h>
#include <windows.h>
#include <sspi.h>
#include <vector>
#include <string>
#include <NTSecAPI.h>
#include <ntsecpkg.h>

#include "utils.h"

namespace utils {
    unsigned int RSHash(const std::string& str)
    {
        unsigned int b = 378551;
        unsigned int a = 63689;
        unsigned int hash = 0;

        for (std::size_t i = 0; i < str.length(); i++)
        {
            hash = hash * a + str[i];
            a = a * b;
        }

        return (hash & 0x7FFFFFFF);
    }

    std::string w2s(const std::wstring& w) {
        std::string s;
        auto size = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
            w.c_str(), w.size(),
            0, 0, 0, 0);

        s.resize(size, '\0');
        if (!WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
            w.c_str(), w.size(),
            LPSTR(s.c_str()), size, "*", 0)) {
            return "";
        }
        return s;
    }

    std::vector<std::string> split(std::string s, std::string delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }



    bool LoadSP(const std::string& path) {
        SECURITY_PACKAGE_OPTIONS spo = {};
        return AddSecurityPackageA((LPSTR)path.c_str(), &spo) != SEC_E_OK;
    }
}
