#pragma once

#include "basetypes.h"
class NTFSParser;

class IndexRecord
{
public:
	IndexRecord(NTFSParser* parser);
	~IndexRecord();
	bool parse(std::vector<char>& buffer);
	bool getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos);

private:
	bool parseEntry(char* buffer);
	bool fixUp(char* buffer);
	NTFSParser* parser_;
	std::vector<FileInfo> files_;
};

