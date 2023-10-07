#pragma once

#include "transport.h"

class Agent {
private:
	Transport* transport;
public:
	Agent(Transport* tr);
	Agent();

	char *StringEncryption(char *text, char *key) noexcept;
	void ClearBuffer(void* buffer, size_t len) noexcept;

	void SetTransport(Transport* transport);

	int Ping();
	int SendCreds(const std::wstring &login, const std::wstring &password) const noexcept;
};


