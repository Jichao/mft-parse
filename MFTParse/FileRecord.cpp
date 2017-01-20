#include "stdafx.h"
#include "Parser.h"
#include "FileRecord.h"

namespace {
	std::vector<DataRunInfo> getRunListInfo(NonResidentAttributeHeader* attrHead) {
		std::vector<DataRunInfo> dataRuns;
		char* runListPtr = (char*)attrHead + attrHead->runListOffset;
		int64_t lcn = 0;
		while (runListPtr < (char*)attrHead + attrHead->size) {
			uint8_t runBytes = 0xf0 & *runListPtr;
			uint8_t countBytes = 0x0f & *runListPtr;
			uint64_t count;
			memcpy(&count, runListPtr + 1, countBytes);
			int64_t runValue;
			char* highBytePtr = runListPtr + countBytes + runBytes;
			bool negative = *highBytePtr & 0x80;
			if (negative) {
				*highBytePtr = *highBytePtr & 0x7f;
			}
			memcpy(&runValue, runListPtr + 1 + countBytes, runBytes);
			if (negative) {
				lcn = lcn - runValue;
			} else {
				lcn = lcn + runValue;
			}
			DataRunInfo dri;
			dri.startLCN = (uint64_t)lcn;
			dri.clusterCount = count;
			dataRuns.push_back(dri);
		}
		return dataRuns;
	}
}

FileRecord::FileRecord(Parser* parser)
{
	parser_ = parser;
}


FileRecord::~FileRecord()
{
}

bool FileRecord::parse(uint64_t offset)
{
	std::vector<char> buffer;
	parser_->getBuffer(offset, parser_->dbr().bytesPerFR(), &buffer);

	if (memcmp(buffer.data(), "FILE", 4) != 0)
		return false;
	header_ = (FileRecordHeader*)(buffer.data());
	if (!fixUp(buffer.data()))
		return false;

	auto attrHead = (NonResidentAttributeHeader*)(header_->attributeOffset + buffer.data());
	while ((char*)attrHead < (char*)header_ + header_->realSize) {
		if (attrHead->type == 0xa0) {
			auto runList = getRunListInfo(attrHead);

		} else {
			attrHead = (NonResidentAttributeHeader *)((char*)attrHead + attrHead->size);
		}
	}
	return true;
}

bool FileRecord::getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos)
{
	return false;
}

bool FileRecord::fixUp(char* buffer)
{
	for (uint16_t i = 0; i < header_->updateSNSize; ++i) {
		int16_t* valuePtr = (int16_t*)(buffer + 512 * i);
		if (*valuePtr != header_->updateNumber) {
			return false;
		}
		*valuePtr = header_->updateArray[i];
	}
	return true;
}
