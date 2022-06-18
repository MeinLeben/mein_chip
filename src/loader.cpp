#include "pch.h"

#include "loader.h"

bool Loader::load(const char* pPath, char** pData, size_t& size) {
	FILE* pFile;
	errno_t err = fopen_s(&pFile, pPath, "r");
	if (err || !pFile) {
		std::cerr << "Failed to open file: " << pPath << std::endl;
		return false;
	}

	fseek(pFile, 0L, SEEK_END);
	size = ftell(pFile);
	rewind(pFile);

	*pData = new char[size];
	size_t num_read = fread_s(*pData, size, sizeof(char), size, pFile);
	if ( num_read != size) {
		std::cerr << "Failed to read data from file: " << pPath << std::endl;
		delete[] (* pData);
		return false;
	}

	return true;
}
