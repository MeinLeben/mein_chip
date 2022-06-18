#pragma once

class Loader
{
public:
	static bool load(const char* path, char** pData, size_t& size);
};

