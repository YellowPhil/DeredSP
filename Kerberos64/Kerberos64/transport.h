#pragma once

#include <winhttp.h>
#include <any>
#include <string>
#include <vector>
#include <ctime>
#include <iostream>

enum CONTENT_TYPE{
	JSON,
	PLAINTEXT,
	RAW_DATA,
	X_WWW_FORM,
};
class Transport {
public:
	virtual int SendData(void *data, size_t dataLen, CONTENT_TYPE contentType) = 0;
};

class HTTPSTransport final: public Transport {
private:
	std::wstring _server_url;
	unsigned long _port;
	HINTERNET _hInet, _hConnection, _hRequest;
	bool isConnected;
public:
	HTTPSTransport(const std::wstring& server_url, unsigned long port);
	HTTPSTransport() = delete;
	~HTTPSTransport();

	int Connect(const std::wstring &verb, const std::wstring &endpoint, const std::wstring &userAgent);
	int SendData(void* data, size_t dataLen, CONTENT_TYPE contentType);
	bool AddHeader(const std::wstring& header, const std::wstring& value);
};
