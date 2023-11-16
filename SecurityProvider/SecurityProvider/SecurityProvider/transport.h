#pragma once

#include <winhttp.h>
#include <any>
#include <string>
#include <vector>
#include <ctime>
#include <iostream>

using ResponseCallback = void(*)(const std::vector<char> &);

enum CONTENT_TYPE{
	JSON,
	PLAINTEXT,
	RAW_DATA,
	X_WWW_FORM,
};

typedef struct {
	HWND        hWindow;        // Handle for the dialog box
	HINTERNET   hConnect;       // Connection handle
	HINTERNET   hRequest;       // Resource request handle
	int         nURL;           // ID of the URL edit box
	int         nHeader;        // ID of the header output box
	int         nResource;      // ID of the resource output box
	DWORD       dwSize;         // Size of the latest data block
	DWORD       dwTotalSize;    // Size of the total data
	LPSTR       lpBuffer;       // Buffer for storing read data
	WCHAR       szMemo[256];    // String providing state information
} REQUEST_CONTEXT;
static REQUEST_CONTEXT rcContext1, rcContext2;


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
	std::vector<char> _responseData;
	ResponseCallback _callback;			// Function that will recieve the response data


	static void CALLBACK Callback(HINTERNET hInet,
								DWORD_PTR dwPtr,
								DWORD status,
								void *statusInfo,
								DWORD statusInfoLen);
public:
	HTTPSTransport(const std::wstring& server_url, unsigned long port);
	HTTPSTransport() = delete;
	~HTTPSTransport();

	int Connect(const std::wstring &verb, const std::wstring &endpoint, const std::wstring &userAgent= L"Mozilla/5.0 (Windows NT 5.1; rv:7.0.1) Gecko/20100101 Firefox/7.0.1");
	int SendData(void* data, size_t dataLen, CONTENT_TYPE contentType);
	int GetData();
	void SetCallback(ResponseCallback);
	int AddHeader(std::wstring header, std::wstring value);
};
