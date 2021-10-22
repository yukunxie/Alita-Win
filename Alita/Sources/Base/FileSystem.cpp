#include "FileSystem.h"


#include <string> 
#include <fstream>
#include <filesystem>

#include <stdio.h>
#include <direct.h>

#if WIN32
#include <windows.h>
#include <locale>
#include <codecvt>
#endif

//#include <boost\filesystem.hpp>


NS_RX_BEGIN

FileSystem* FileSystem::instance_ = nullptr;

#if WIN32
std::string GetExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	std::wstring path = std::wstring(buffer).substr(0, pos);

	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.to_bytes(path);
}
#endif

FileSystem::FileSystem()
{
	char workingDir[1024] = { 0 };
	getcwd(workingDir, 1024);
	if (workingDir[0])
	{
		searchPaths_.push_back(workingDir);
	}
	WorkingDirectory_ = workingDir;
	GameDataDirectory_ = WorkingDirectory_ + "/GameData/";
	if (!std::filesystem::is_directory(GameDataDirectory_))
	{
		std::filesystem::create_directories(GameDataDirectory_);
	}
	/*if (!boost::filesystem::is_directory(GameDataDirectory_))
	{
		boost::filesystem::create_directories(GameDataDirectory_);
	}*/


	std::string sWorkingDir = std::string(workingDir);
	searchPaths_.push_back(std::string(workingDir) + "/Assets/");
	searchPaths_.push_back(std::string(workingDir) + "/Sources/");

	{
		std::string::size_type pos = sWorkingDir.rfind("\\Alita");
		if (pos != std::string::npos)
		{
			std::string basePath = sWorkingDir.substr(0, pos);
			searchPaths_.push_back(std::string(basePath) + "/Alita/Assets/");
			searchPaths_.push_back(std::string(basePath) + "/Alita/Sources/");
		}
	}

	// todo realxie
#if defined(WIN32) && defined(_DEBUG)
	//searchPaths_.push_back(workingDir + std::string("/x64/Debug"));
	searchPaths_.push_back("x64/Debug");
#endif
}

FileSystem* FileSystem::GetInstance()
{
	if (!FileSystem::instance_)
	{
		FileSystem::instance_ = new FileSystem();
	}
	return FileSystem::instance_;
}

void FileSystem::AppendSearchPath(const char* searchPath)
{
	searchPaths_.push_back(searchPath);
}

std::string FileSystem::GetAbsFilePath(const char* filename)
{
	for (const std::string& dir : searchPaths_)
	{
		std::string absFilePath = dir + "/" + filename;
		std::ifstream input(absFilePath, std::ios::binary);
		if (input.is_open())
		{
			input.close();
			return absFilePath;
		}
	}
	return "";
}


TData FileSystem::GetBinaryData(const char* filename)
{
	const std::string& absFilePath = GetAbsFilePath(filename);
	std::ifstream input(absFilePath.c_str(), std::ios::binary);
	if (!input.is_open())
	{
		LOGE("file [%] open fail!", filename);
		return {};
	}
	input.seekg(0, std::ios::end);
	int size = input.tellg();
	input.seekg(0, std::ios::beg);
	std::vector<std::uint8_t> data(size, 0);
	
	input.read((char*)data.data(), size);
	return std::move(data);
}

std::string FileSystem::GetStringData(const char* filename)
{
	const TData& data = GetBinaryData(filename);
	std::string ret = "";
	ret.append((const char*)data.data(), data.size());
	return ret;
}

NS_RX_END