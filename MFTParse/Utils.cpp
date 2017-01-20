#include "stdafx.h"
#include "Utils.h"


Utils::Utils()
{
}


Utils::~Utils()
{
}

std::vector<DataRunInfo> Utils::GetRunListInfo(NonResidentAttributeHeader* attrHead)
{
	std::vector<DataRunInfo> dataRuns;
	char* runListPtr = (char*)attrHead + attrHead->runListOffset;
	int64_t lcn = 0;
	uint64_t vcn = 0;
	while (runListPtr < (char*)attrHead + attrHead->size) {
		uint8_t runBytes = 0xf0 & *runListPtr;
		uint8_t countBytes = 0x0f & *runListPtr;
		uint64_t count;
		memcpy(&count, runListPtr + 1, countBytes);
		int64_t runValue;
		char* highBytePtr = runListPtr + countBytes + runBytes;
		bool negative = !!(*highBytePtr & 0x80);
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
		dri.vcn = vcn;
		dri.lcn = (uint64_t)lcn;
		dri.count = count;
		vcn += count;
		dataRuns.push_back(dri);
	}
	return dataRuns;
}

uint64_t Utils::RefToNo(uint64_t referenceNumber)
{
	return (referenceNumber & 0x0000ffffffffffff);
}

