#include <iostream>
#include "Packer.h"


void test() {
	throw std::invalid_argument("ERROR");
}

int main() {
	Packer packer{};
	try {
		test();
	}
	catch (std::exception e) {
		std::cerr << e.what() << "\n\n\n";
	}
	std::cout << "ASDFASDFASDFASDFASDFASDFASDF\n\n\n\n";
}