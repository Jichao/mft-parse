#include "stdafx.h"
#include "IndexRecord.h"
#include "FileRecord.h"
#include "NTFSParser.h"
#include "utils.h"

IndexRecord::IndexRecord(NTFSParser* paser)
{
	parser_ = paser;
}

IndexRecord::~IndexRecord()
{
}

bool IndexRecord::parse(std::vector<char>& buffer)
{
	char* ptr = buffer.data();
	while (ptr < buffer.data() + buffer.size()) {
		if (!parseEntry(ptr)) {
			return false;
		}
		ptr += parser_->dbr().bytesPerIndexRecord();
	}
	return true;
}

bool IndexRecord::parseEntry(char* buffer)
{
	if (memcmp(buffer, "INDX", 4) != 0)
		return false;

	if (!fixUp(buffer))
		return false;

	IndexHeader* header = (IndexHeader*)buffer;
	auto entryHead = (IndexEntryHeader*)((char*)&header->indexEntryOffset + header->indexEntryOffset);
	while ((char*)entryHead < (char*)header + header->indexEntrySize) {
		IndexFileAttribute* fileAttr = (IndexFileAttribute*)((char*)entryHead + 0x10);
		FileInfo fi;
		fi.accessTime = fileAttr->lastAccessTime;
		fi.createTime = fileAttr->createTime;
		fi.modifyTime = fileAttr->modifiedTime;
		fi.size = fileAttr->realSize;
		fi.allocedSize = fileAttr->allocedSize;
		fi.name.assign(fileAttr->filename, fileAttr->filenameLength);
		fi.flags = fileAttr->flags;
		fi.referenceNumber = entryHead->referenceNumber;
		printf("%S\n", fi.name.c_str());
		if (fi.isDir()) {
			parser_->getFR(Utils::RefToNo(entryHead->referenceNumber));
		}
		entryHead = (IndexEntryHeader*)((char*)entryHead + entryHead->entryLength);
	}
	return true;
}

bool IndexRecord::fixUp(char* buffer)
{
	IndexHeader* header = (IndexHeader*)(buffer);
	for (uint16_t i = 1; i < header->updateSNSize; ++i) {
		int16_t* valuePtr = (int16_t*)(buffer + 512 * i - 2);
		if (*valuePtr != header->updateNumber) {
			return false;
		}
		*valuePtr = header->updateArray[i];
	}
	return true;
}

bool IndexRecord::getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos)
{
	if (dir.empty()) {
		*fileInfos = files_;
		return true;
	}

	auto name = dir.substr(0, dir.find_first_of(L"\\"));
	auto entry = std::find_if(files_.begin(), files_.end(), [name](const FileInfo& fi) {
		return lstrcmpi(fi.name.c_str(), name.c_str()) == 0;
	});
	if (entry == files_.end())
		return false;
	return parser_->getFR(entry->referenceNumber)->getFilesInDir(
		dir.substr(dir.find_first_of(L"\\") + 1), fileInfos);
}
