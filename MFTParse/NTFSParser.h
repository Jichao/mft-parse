#pragma once

#include "basetypes.h"

class FileRecord;

class NTFSParser
{
public:
	static NTFSParser* GetParserForDriver(wchar_t driver);
	~NTFSParser();
	bool getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos);
	bool init(wchar_t driver);
	DBR& dbr();
	bool getBuffer(uint64_t offset, uint32_t size, std::vector<char>* buffer);
	bool getFRBuffer(uint64_t frNumber, std::vector<char>* buffer);
	FileRecord* getFR(uint64_t frNumber);

private:
	bool parseAttr();
	NTFSParser();

	bool dumpDBR();
	DBR dbr_;
	HANDLE hFile_;
	std::map<uint64_t, FileRecord*> frs_;

	static std::map<wchar_t, NTFSParser*> s_parsers;
};

