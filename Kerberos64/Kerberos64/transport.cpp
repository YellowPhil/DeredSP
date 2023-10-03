#include <string>
#include <sstream>
#include <windows.h>
#include <any>

#include "transport.h"
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "user32.lib")

HTTPSTransport::HTTPSTransport(const std::wstring& server_url): _server_url(server_url) {}

bool HTTPSTransport::init() {
		return false;
}

HTTPSTransport::~HTTPSTransport() {
		WinHttpCloseHandle(_hInet);
	}

int HTTPSTransport::Ping(std::time_t time) {
	std::wstringstream ss;
	ss << time;
	std::wstring stringTime{ ss.str() };

	auto hInet = WinHttpOpen(L"A WinHTTP Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	auto hConnect = WinHttpConnect(hInet, _server_url.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
	if (!hConnect) {
		return 1;
	}
	auto hRequest = WinHttpOpenRequest(hConnect,
		_PostVerb.c_str(),
		_pingEndpoint.c_str(),
		NULL,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_ESCAPE_PERCENT |WINHTTP_FLAG_REFRESH );
	if (!hRequest) {
		return 2;
	}

	auto bResult = WinHttpSendRequest(hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS,
		NULL,
		LPVOID(stringTime.c_str()),
		stringTime.length(),
		stringTime.length(),
		NULL);
	if (!bResult) {
		return 3;
	}
	return 0;
}

int HTTPSTransport::SendData(const std::wstring& endpoint, LPVOID data, size_t dataLen) {
	auto hInet = WinHttpOpen(L"A WinHTTP Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	auto hConnect = WinHttpConnect(hInet, _server_url.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
	if (!hConnect) {
		return 1;
	}
	auto hRequest = WinHttpOpenRequest(hConnect,
		_PostVerb.c_str(),
		_pingEndpoint.c_str(),
		NULL,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_ESCAPE_PERCENT |WINHTTP_FLAG_REFRESH );
	if (!hRequest) {
		return 2;
	}
}
    