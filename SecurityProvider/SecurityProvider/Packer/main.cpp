#include <iostream>
#include "Packer.h"


int main() {
	Packer packer{};
	try {
		packer.Pack("Z:\\repos\\SecurityProvider\\SecurityProvider\\x64\\Debug\\Agent.exe", "Z:\\repos\\SecurityProvider\\SecurityProvider\\x64\\SP.dll", "C:\\Users\\PEESEE\\Downloads\\Nigger.exe");
	}
	catch (std::exception e) {
		std::cerr << e.what();
	}
}