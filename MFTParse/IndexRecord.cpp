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
	if (memcmp(buffer.data(), "INDX", 4) != 0)
		return false;
	header_ = (IndexHeader*)(buffer.data());
	if (!fixUp(buffer.data()))
		return false;

	auto entryHead = (IndexEntryHeader*)((char*)&header_->indexEntryOffset + header_->indexEntryOffset);
	while ((char*)entryHead < (char*)header_ + header_->indexEntrySize) {
		IndexFileAttribute* fileAttr = (IndexFileAttribute*)((char*)entryHead + 10);
		FileInfo fi;
		fi.accessTime = fileAttr->lastAccessTime;
		fi.createTime = fileAttr->createTime;
		fi.modifyTime = fileAttr->modifiedTime;
		fi.size = fileAttr->realSize;
		fi.allocedSize = fileAttr->allocedSize;
		fi.name.assign(fileAttr->filename, fileAttr->filenameLength);
		fi.flags = fileAttr->flags;
		fi.referenceNumber = entryHead->referenceNumber;
		if (fi.isDir()) {
			parser_->getFR(Utils::RefToNo(entryHead->referenceNumber));
		}
	}
	return true;
}

bool IndexRecord::fixUp(char* buffer)
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

