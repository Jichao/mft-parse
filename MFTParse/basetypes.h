#pragma once

#include <stdint.h>

struct FileInfo {
	uint64_t referenceNumber;
	uint8_t flags;
	time_t createTime;
	time_t modifyTime;
	time_t accessTime;
	uint64_t size;
	uint64_t allocedSize;
	std::wstring name;
	bool isDir() {
		return !!(flags & FILE_ATTRIBUTE_DIRECTORY);
	}
};

#pragma pack(1)
struct DBR {
	char jmp[3];
	char oem[8];
	uint16_t bytesPerSector;
	uint8_t sectorsPerCluster;
	char reserverd1[7];
	char diskType;
	char reserverd2[2];
	uint16_t sectorsPerTrack;
	uint16_t headCount;
	uint16_t hiddenSectors;
	char reserved3[8];
	uint64_t sectorCount;
	uint64_t mftStartLCN;
	uint64_t mftMirrStartLCN;
	int8_t clustersPerFileRecord;
	char reserved4[3];
	int8_t clustersPerIndexBuffer;
	char reserved5[3];
	uint64_t volumeNumber;
	uint32_t checksum;
	uint32_t bytesPerFR() {
		if (clustersPerFileRecord < 0) {
			return 2 << (-1 * clustersPerFileRecord);
		} else {
			return clustersPerFileRecord * bytesPerCluster();
		}
	}
	uint32_t bytesPerCluster() {
		return bytesPerSector * sectorsPerCluster;
	}
};

struct FileRecordHeader
{
	char magicNumber[4];
	uint16_t updateSNOffset;
	uint16_t updateSNSize;
	uint64_t LSN;
	uint16_t SN;
	uint16_t hardLinkCount;
	uint16_t attributeOffset;
	uint16_t flag;
	uint16_t realSize;
	uint16_t allocedSize;
	uint64_t baseReferenceNumber;
	uint16_t nextAttributeID;
	uint16_t xpBoundary;
	uint32_t xpNumber;
	uint16_t updateNumber;
	uint16_t updateArray[1];
};

struct ResidentAttributeHeader
{
	uint32_t type;
	uint32_t size;
	uint8_t nonResident;
	uint8_t nameLength;
	uint16_t nameOffset;
	uint16_t flag;
	uint16_t ID;
	uint32_t attrLen;
	uint16_t attrOffset;
	uint8_t indexFlag;
	char reserved[1];
};

struct NonResidentAttributeHeader
{
	uint32_t type;
	uint32_t size;
	uint8_t nonResident;
	uint8_t nameLength;
	uint16_t nameOffset;
	uint16_t flag;
	uint16_t ID;
	uint64_t startVCN;
	uint64_t endVCN;
	uint16_t runListOffset;
	uint16_t compressUnit;
	char reserved[4];
	uint64_t attrAllocSize;
	uint64_t attrInitSize;
};

struct DataRunInfo
{
	uint64_t lcn;
	uint64_t vcn;
	uint64_t count;
	DataRunInfo() {
		lcn = 0;
		vcn = 0;
		count = 0;
	}
};

struct IndexHeader
{
	char magicNumber[4];
	uint16_t updateSNOffset;
	uint16_t updateSNSize;
	uint64_t LSN;
	uint64_t VCN;
	uint32_t indexEntryOffset;
	uint32_t indexEntrySize;
	uint32_t indexEntryAllocedSize;
	uint8_t hasChild;
	char padding[3];
	uint16_t updateNumber;
	uint16_t updateArray[1];
};

struct IndexEntryHeader
{
	uint64_t referenceNumber;
	uint16_t entryLength;
	uint16_t streamLength;
	uint8_t flag;
	uint8_t padding[3];
};

struct IndexFileAttribute 
{
	uint64_t parentReferenceNumber;
	uint64_t createTime;
	uint64_t modifiedTime;
	uint64_t MFTChangeTime;
	uint64_t lastAccessTime;
	uint64_t allocedSize;
	uint64_t realSize;
	uint32_t flags;
	uint32_t reparse;
	uint8_t filenameLength;
	uint8_t filenameSpace;
	wchar_t filename[1];
};

#pragma pack()
