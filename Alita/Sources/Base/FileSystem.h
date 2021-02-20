#pragma once

#include "Macros.h"

#include <vector>
#include <string>

NS_RX_BEGIN

class FileSystem
{
protected:
	FileSystem();

public:
	virtual ~FileSystem() {}

	void AppendSearchPath(const char* searchPath);

public:
	static FileSystem* GetInstance();

	std::string GetAbsFilePath(const char* filename);

	std::vector<std::uint8_t> GetFileData(const char* filename);

private:
	static FileSystem* instance_;

	std::vector<std::string> searchPaths_;
};

NS_RX_END

