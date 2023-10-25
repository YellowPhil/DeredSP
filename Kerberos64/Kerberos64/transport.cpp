#include <string>
#include <sstream>
#include <windows.h>
#include <any>

#include "transport.h"
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

HTTPSTransport::HTTPSTransport(const std::wstring &server_url, unsigned long port):
	_server_url(server_url), _port(port) {
	isConnected = false;
	_hConnection = _hInet = _hRequest = NULL;
}

int HTTPSTransport::Connect(const std::wstring &verb, const std::wstring &endpoint, 
	const std::wstring& userAgent)
 {
	_hInet = WinHttpOpen(userAgent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_SECURE_DEFAULTS);
	if (!_hInet) {
		return GetLastError();
	}
	_hConnection = WinHttpConnect(_hInet, _server_url.c_str(), _port, 0);
	if (!_hConnection) {
		return GetLastError();
	}
	_hRequest = WinHttpOpenRequest(_hConnection,
		verb.c_str(),
		endpoint.c_str(),
		NULL,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_ESCAPE_PERCENT |WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE
	);
	if (!_hRequest) {
		return GetLastError();
	}
	isConnected = true;
	return 0;
}

int HTTPSTransport::SendData(void* data, size_t dataLen, CONTENT_TYPE contentType) {
	if (!isConnected) return 123;

	std::wstring ct = L"Content-Type";
	switch (contentType) {
	case JSON:
		AddHeader(ct, L"application/json");
		break;
	case PLAINTEXT:
		AddHeader(ct, L"text/plain");
		break;
	case RAW_DATA:
		break;
	case X_WWW_FORM:
		AddHeader(ct, L"application/x-www-form-urlencoded");
		break;
	default:
		AddHeader(ct, L"text/plain");
	}

	auto bResult = WinHttpSendRequest(_hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS,
		NULL,
		data,
		dataLen,
		dataLen,
		NULL);
	return bResult == false;
}

int HTTPSTransport::AddHeader(std::wstring header, std::wstring value) {
	if (!isConnected) return 123;

	std::wstring headerString = header + L": " + value;
	auto bResult = WinHttpAddRequestHeaders(
		_hRequest,
		headerString.c_str(),
		DWORD(-1),
		WINHTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA
	);
	return bResult == false;


}

HTTPSTransport::~HTTPSTransport() {
	if (_hRequest) WinHttpCloseHandle(_hRequest);
	if (_hConnection) WinHttpCloseHandle(_hConnection);
	if (_hInet) WinHttpCloseHandle(_hInet);
}

//void *Transport::Encryption(char *text, char*key) {
//}
//
//HTTPSTransport::HTTPSTransport(const std::wstring& server_url) : _server_url(server_url) {};
//
//int HTTPSTransport::Ping(std::time_t time) {
//	HTTPSTransport::counter++;
//	std::wstringstream ss;
//	HINTERNET hInet{NULL}, hConnect{NULL}, hRequest{NULL};
//	BYTE bResult{ FALSE };
//
//	ss << time;
//	std::wstring stringTime{ ss.str() };
//	std::string stringcounter{};
//
//	hInet = WinHttpOpen(L"A WinHTTP Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_SECURE_DEFAULTS);
//	hConnect = WinHttpConnect(hInet, _server_url.c_str(), 4443, 0);
//	if (!hConnect) {
//		goto returnWithError;
//	}
//	hRequest = WinHttpOpenRequest(hConnect,
//		_PostVerb.c_str(),
//		_pingEndpoint.c_str(),
//		NULL,
//		WINHTTP_NO_REFERER,
//		WINHTTP_DEFAULT_ACCEPT_TYPES,
//		WINHTTP_FLAG_ESCAPE_PERCENT |WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE);
//	if (!hRequest) {
//		goto returnWithError;
//	}
//
//	stringcounter = std::to_string(HTTPSTransport::counter);
//
//	bResult = WinHttpSendRequest(hRequest,
//		WINHTTP_NO_ADDITIONAL_HEADERS,
//		NULL,
//		//LPVOID(stringTime.c_str()),
//		LPVOID(stringcounter.c_str()),
//		stringcounter.length() * sizeof(char),
//		stringcounter.length() * sizeof(char),
//		NULL);
//	if (!bResult) {
//		goto returnWithError;
//	}
//	return 0;
//
//returnWithError:
//	if (hRequest) WinHttpCloseHandle(hRequest);
//	if (hConnect) WinHttpCloseHandle(hConnect);
//	if (hInet) WinHttpCloseHandle(hInet);
//	return GetLastError();
//}
//
//int HTTPSTransport::SendData(const std::wstring& endpoint, LPVOID data, size_t dataLen) {
//	HINTERNET hInet{ NULL }, hConnect{ NULL }, hRequest(NULL);
//	BYTE bResult{ FALSE };
//
//	hInet = WinHttpOpen(L"A WinHTTP Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_SECURE_DEFAULTS);
//	hConnect = WinHttpConnect(hInet, _server_url.c_str(), 4443, 0);
//
//	if (!hConnect) {
//		goto returnWithError;
//	}
//	hRequest = WinHttpOpenRequest(hConnect,
//		_PostVerb.c_str(),
//		endpoint.c_str(),
//		NULL,
//		WINHTTP_NO_REFERER,
//		WINHTTP_DEFAULT_ACCEPT_TYPES,
//		WINHTTP_FLAG_ESCAPE_PERCENT | WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE);
//	if (!hRequest) {
//		goto returnWithError;
//	}
//
//	if (!WinHttpAddRequestHeaders(hRequest,
//		_postHeaders.c_str(),
//		_postHeaders.length(),
//		WINHTTP_ADDREQ_FLAG_REPLACE | WINHTTP_ADDREQ_FLAG_ADD)) {
//		goto returnWithError;
//	}
//
//	bResult = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, NULL, data, dataLen, dataLen, NULL);
//	if (!bResult) {
//		goto returnWithError;
//	}
//	return 0;
//
//returnWithError:
//	if (hRequest) WinHttpCloseHandle(hRequest);
//	if (hConnect) WinHttpCloseHandle(hConnect);
//	if (hInet) WinHttpCloseHandle(hInet);
//	return GetLastError();
//}
