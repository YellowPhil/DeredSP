#include <string>
#include <sstream>
#include <windows.h>
#include <any>

#include "transport.h"
#include <winhttp.h>

HTTPSTransport::HTTPSTransport(const std::wstring &server_url, unsigned long port):
	_server_url(server_url), _port(port) {
	isConnected = false;
	_hConnection = _hInet = _hRequest = NULL;
}

int HTTPSTransport::Connect(const std::wstring &verb, const std::wstring &endpoint, 
	const std::wstring& userAgent)
 {
	_hInet = WinHttpOpen(userAgent.c_str(),
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		WINHTTP_FLAG_SECURE_DEFAULTS | WINHTTP_FLAG_ASYNC);
	if (!_hInet) {
		return GetLastError();
	}

	if (_callback) {
		if (WinHttpSetStatusCallback(_hInet, Callback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK) {
			return  123;
		}
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


int HTTPSTransport::GetData() {
	if (!isConnected) return 123;

	auto bResult = WinHttpSendRequest(_hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS,
		0,
		WINHTTP_NO_REQUEST_DATA,
		0, 0, 0);
	if (!bResult) {
		return 3;
	}
	if (!WinHttpReceiveResponse(_hRequest, 0)) {
		return 12;
	}
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

	if (byte bResult = WinHttpSendRequest(_hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS,
		NULL,
		data,
		dataLen,
		dataLen,
		NULL); bResult == false) {
		return 3;
	}
	return 0;
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

void CALLBACK HTTPSTransport::Callback(
		HINTERNET hInternet,
		DWORD_PTR dwContext,
		DWORD dwInternetStatus,
		void *lpvStatusInformation,
		DWORD dwStatusInformationLength) {
	DWORD bytesAvaliable = 0;
	std::cout << "DATA AVALIABLE: " << std::hex << dwInternetStatus << "\n";
	auto transport = reinterpret_cast<HTTPSTransport*>(dwContext);

	switch (dwInternetStatus) {
		case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
			if (!WinHttpReceiveResponse(hInternet, 0)) {
			}
			break;
		case WINHTTP_CALLBACK_FLAG_HEADERS_AVAILABLE:
			break;
		case WINHTTP_CALLBACK_FLAG_DATA_AVAILABLE :
			
			if (WinHttpQueryDataAvailable(hInternet, &bytesAvaliable) && bytesAvaliable > 0) {
				std::vector<char> buffer(bytesAvaliable);

				if (WinHttpReadData(hInternet, buffer.data(), bytesAvaliable, 0)) {
					transport->_responseData.insert(transport->_responseData.end(),
						buffer.begin(), buffer.end());
				}
				else {
					WinHttpCloseHandle(hInternet);
				}
			}
			
			break;
		case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
			/*
			WINHTTP_ASYNC_RESULT *result = reinterpret_cast<WINHTTP_ASYNC_RESULT*>(lpvStatusInformation);
			if (result->dwError == ERROR_SUCCESS) {
				auto bytesRead = result->dwResult;
				if (bytesRead == 0) {
					transport->_callback(transport->_responseData);
				}
			}
			*/
			break;
		case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
			if (WinHttpQueryDataAvailable(hInternet, &bytesAvaliable) && bytesAvaliable > 0) {
				std::vector<char> nigger(bytesAvaliable);
				if (WinHttpReadData(hInternet, nigger.data(), bytesAvaliable, 0)) {
					std::cout << nigger.data();
				}
				else {
					WinHttpCloseHandle(hInternet);
				}
			}
			else {
				std::cout << "No data for you";
			}
			break;
		default:
			break;
	}
	
}

void HTTPSTransport::SetCallback(ResponseCallback callback) {
	_callback = callback;
}
HTTPSTransport::~HTTPSTransport() {
	//if (_hRequest) WinHttpCloseHandle(_hRequest);
	//if (_hConnection) WinHttpCloseHandle(_hConnection);
	//if (_hInet) WinHttpCloseHandle(_hInet);
}