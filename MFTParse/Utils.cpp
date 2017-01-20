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
	if (attrHead->nonResident) {
		char* runListPtr = (char*)attrHead + attrHead->runListOffset;
		int64_t lcn = 0;
		uint64_t vcn = 0;
		while (runListPtr < (char*)attrHead + attrHead->size) {
			uint8_t runBytes = 0x0f & (*runListPtr >> 4);
			uint8_t countBytes = 0x0f & *runListPtr;
			if (runBytes == 0 || countBytes == 0)
				break;
			assert(runBytes <= 8);
			assert(countBytes <= 4);

			uint64_t count = 0;
			memcpy(&count, runListPtr + 1, countBytes);

			char* highBytePtr = runListPtr + countBytes + runBytes;
			bool negative = !!(*highBytePtr & 0x80);
			if (negative) {
				*highBytePtr = *highBytePtr & 0x7f;
			}
			int64_t runValue = 0;
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
			runListPtr += runBytes + countBytes + 1;
		}
	}
	return dataRuns;
}

uint64_t Utils::RefToNo(uint64_t referenceNumber)
{
	return (referenceNumber & 0x0000ffffffffffff);
}

