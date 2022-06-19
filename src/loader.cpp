#include "pch.h"

#include "loader.h"

bool Loader::load(const char* pPath, uint8_t** pData, size_t& size) {
	std::ifstream fs(pPath, std::ifstream::binary);
	if (!fs.is_open()) {
		std::cerr << "Failed to open file: " << pPath << std::endl;
		return false;
	}

	fs.seekg(0, fs.end);
	size = fs.tellg();
	fs.seekg(0, fs.beg);

	*pData = new uint8_t[size];
	fs.read((char*)(*pData), size);

	fs.close();

	return true;
}
