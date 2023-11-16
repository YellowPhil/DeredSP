#pragma once
#include <sstream>
#include <iostream>

class StreamBufRedirect
{
private:
	std::ostream& originalStream;
	std::streambuf* originalBuffer;
public:
	StreamBufRedirect(std::ostream &coutStream, std::streambuf* newBuf);
	~StreamBufRedirect();
};

