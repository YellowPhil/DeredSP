#ifndef _TRANSPORT_HEADER
#define _TRANSPORT_HEADER

#include <winhttp.h>
#include <any>
#include <string>
#include <ctime>
#include <iostream>

class Transport {
public:
	virtual int Ping(std::time_t time) = 0;
	virtual int SendData(const std::wstring &endpoint, LPVOID data) = 0;
};

class HTTPSTransport : public Transport {
private:
	std::wstring _server_url;
	std::wstring _PostVerb = L"POST";
	std::wstring _pingEndpoint = L"/ping";
	HINTERNET _hInet;
public:
	HTTPSTransport(const std::wstring& server_url);
	HTTPSTransport() = default;
	~HTTPSTransport();

	bool init();
	int Ping(std::time_t time);
	int SendData(const std::wstring& endpoint, LPVOID data, size_t dataLen);
};

#endif // !_TRANSPORT_HEADER
