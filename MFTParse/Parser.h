#pragma once

#include "basetypes.h"

class FileRecord;

class Parser
{
public:
	static Parser* GetParserForDriver(wchar_t driver);
	~Parser();
	bool getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos);
	bool init(wchar_t driver);
	DBR& dbr();
	bool getBuffer(uint64_t offset, uint32_t size, std::vector<char>* buffer);

private:
	bool parseAttr();
	Parser();

	bool dumpDBR();
	DBR dbr_;
	HANDLE hFile_;
	std::unique_ptr<FileRecord> rootFR_;
	static std::map<wchar_t, Parser*> s_parsers;
};

