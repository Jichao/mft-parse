#pragma once

#include <stdint.h>

struct FileInfo {
	bool isDir;
	time_t createTime;
	time_t modifyTime;
	time_t accessTime;
	std::wstring name;
};

struct DBR {
	char oem[4];
	char jmp[3];
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
	uint64_t basicIndexNumber;
	uint16_t nextAttributeID;
	uint16_t boundary;
	uint32_t referenceNumber;
	uint16_t updateNumber;
	uint16_t updateArray[0];
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
	uint64_t startLCN;
	uint64_t clusterCount;
	DataRunInfo() {
		startLCN = 0;
		clusterCount = 0;
	}
};
