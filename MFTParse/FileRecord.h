#pragma once

#include "basetypes.h"

class Parser;

class FileRecord
{
public:
	FileRecord(Parser* parser);
	~FileRecord();
	bool parse(uint64_t offset);
	bool getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos);

private:
	bool fixUp(char* buffer);
	FileRecordHeader* header_;
	Parser* parser_;
};

