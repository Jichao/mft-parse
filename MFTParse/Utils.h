#pragma once

#include "basetypes.h"

class Utils
{
public:
	Utils();
	~Utils();
	static std::vector<DataRunInfo> GetRunListInfo(NonResidentAttributeHeader* attrHead);
	static uint64_t RefToNo(uint64_t referenceNumber);
};

