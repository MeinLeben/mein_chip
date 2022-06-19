#pragma once

class Loader
{
public:
	static bool load(const char* pPath, uint8_t** pData, size_t& size);
};

