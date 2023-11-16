#include "StreamBufRedirect.h"
#include <sstream>
#include <iostream>

StreamBufRedirect::StreamBufRedirect(std::ostream& coutStream, std::streambuf* newBuf): originalBuffer(coutStream.rdbuf()), originalStream(coutStream) {
	coutStream.rdbuf(newBuf);
}

StreamBufRedirect::~StreamBufRedirect() {
	originalStream.rdbuf(originalBuffer);
}
