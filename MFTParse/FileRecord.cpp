#include "stdafx.h"
#include "NTFSParser.h"
#include "FileRecord.h"
#include "IndexRecord.h"
#include "utils.h"

FileRecord::FileRecord(NTFSParser* parser)
{
	parser_ = parser;
}


FileRecord::~FileRecord()
{
}

bool FileRecord::parse(uint64_t frNumber)
{
	std::vector<char> buffer;
	parser_->getFRBuffer(frNumber, &buffer);

	if (memcmp(buffer.data(), "FILE", 4) != 0)
		return false;
	header_ = (FileRecordHeader*)(buffer.data());
	if (!fixUp(buffer.data()))
		return false;

	auto attrHead = (NonResidentAttributeHeader*)(header_->attributeOffset + buffer.data());
	while ((char*)attrHead < (char*)header_ + header_->realSize) {
		if (attrHead->type == 0xa0 || attrHead->type == 0x80) {
			auto runList = Utils::GetRunListInfo(attrHead);
			std::vector<char> buffer;
			if (!readRunList(runList, &buffer)) {
				return false;
			}
			if (attrHead->type == 0xa0) {
				indexRecord_ = new IndexRecord(parser_);
				indexRecord_->parse(buffer);
			} else if (attrHead->type == 0x80) {
				dataRunList_ = runList;
			}
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

bool FileRecord::readRunList(const std::vector<DataRunInfo>& runList, std::vector<char>* buffer)
{
	for (auto run : runList) {
		if (!parser_->getBuffer(run.lcn * bytesPerCluster(),
			(uint32_t)run.count * bytesPerCluster(), buffer))
			return false;
	}
	return true;
}

bool FileRecord::getData(uint64_t offset, uint32_t size, std::vector<char>* buffer)
{
	uint64_t startVCN = offset / (bytesPerCluster());
	uint32_t startOffset = offset % (bytesPerCluster());
	uint64_t endVCN = (offset + size) / (bytesPerCluster());
	uint32_t endOffset = (offset + size) % (bytesPerCluster());
	uint64_t lcn = 0;

	auto startRun = std::find_if(dataRunList_.begin(), dataRunList_.end(), [startVCN](const DataRunInfo& dri) {
		return startVCN >= dri.vcn && startVCN < dri.vcn + dri.count;
	});
	auto endRun = std::find_if(dataRunList_.begin(), dataRunList_.end(), [endVCN](const DataRunInfo& dri) {
		return endVCN >= dri.vcn && endVCN < dri.vcn + dri.count;
	});
	if (startRun == dataRunList_.end() || endRun == dataRunList_.end())
		return false;
	if (startRun == endRun) {
		uint64_t roffset = (startRun->lcn + (startVCN - startRun->vcn)) * bytesPerCluster() + startOffset;
		return parser_->getBuffer(roffset, size, buffer);
	} else {
		for (auto run = startRun;; run++) {
			if (run == startRun) {
				uint64_t roffset = (startRun->lcn + (startVCN - startRun->vcn)) * bytesPerCluster() + startOffset;
				if (!parser_->getBuffer(roffset, size, buffer)) {
					return false;
				}
			} else if (run == endRun) {
				uint64_t roffset = (endRun->lcn + (endVCN - endRun->vcn)) * bytesPerCluster();
				if (!parser_->getBuffer(roffset, endOffset, buffer)) {
					return false;
				}
				break;
			} else {
				uint64_t roffset = run->lcn * bytesPerCluster();
				if (!parser_->getBuffer(roffset, (uint32_t)run->count * bytesPerCluster(), buffer)) {
					return false;
				}
			}
		}
	}
	return true;
}

uint32_t FileRecord::bytesPerCluster()
{
	return parser_->dbr().bytesPerCluster();
}
