#pragma once

#include "basetypes.h"

class NTFSParser;
class IndexRecord;
class FileRecord
{
public:
	FileRecord(NTFSParser* parser);
	~FileRecord();
	bool parse(uint64_t frNumber);
	bool getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos);
	bool getData(uint64_t offset, uint32_t size, std::vector<char>* buffer);

private:
	bool fixUp(char* buffer);
	bool readRunList(const std::vector<DataRunInfo>& runList, std::vector<char>* buffer);
	uint32_t bytesPerCluster();

	FileRecordHeader* header_;
	IndexRecord* indexRecord_;
	NTFSParser* parser_;
	std::vector<DataRunInfo> dataRunList_;
};

