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

	int Connect(const std::wstring &verb, const std::wstring &endpoint, const std::wstring &userAgent= L"Mozilla/5.0 (Windows NT 5.1; rv:7.0.1) Gecko/20100101 Firefox/7.0.1");
	int SendData(void* data, size_t dataLen, CONTENT_TYPE contentType);
	int AddHeader(std::wstring header, std::wstring value);
};
