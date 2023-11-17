#include <tchar.h>
#include "Override.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int _tmain(int argc, _TCHAR** argv)
{
    Override overrider;
    if (overrider.ReplaceImage(target_inject.c_str(), pe_inject, self_injection)) {
      std::wprintf(L"[+] %s injected sucessfully! [+]\n", pe_inject);
        return EXIT_SUCCESS;
    } 

}

