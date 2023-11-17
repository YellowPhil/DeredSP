#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<stdlib.h>
#include <wchar.h>
#include <windows.h>



int main() {
    char* computerFQDN = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, BUFSIZ);
        DWORD dwSize = BUFSIZ;
        GetComputerNameExA(ComputerNamePhysicalDnsFullyQualified, 
            computerFQDN, &dwSize);
        unsigned int clientHash = RSHash(computerFQDN);
        wchar_t ClientIDHeader[BUFSIZ] = { 0 };
        swprintf(ClientIDHeader, BUFSIZ, L"X-Client-ID: %lu", clientHash);
        wprintf(L"%s", ClientIDHeader);
}
