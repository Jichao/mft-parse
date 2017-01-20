#include "stdafx.h"
#include "NTFSParser.h"
#include "StringUtils.h"
#include "FileRecord.h"

std::map<wchar_t, NTFSParser*> NTFSParser::s_parsers;

NTFSParser* NTFSParser::GetParserForDriver(wchar_t driver)
{
	if (!s_parsers.count(driver)) {
		NTFSParser* parser = new NTFSParser();
		if (!parser->init(driver)) {
			delete parser;
			return nullptr;
		} else {
			s_parsers[driver] = parser;
		}
	} 
	return s_parsers[driver];
}

NTFSParser::NTFSParser()
{
}


NTFSParser::~NTFSParser()
{
}

bool NTFSParser::init(wchar_t driver)
{
	hFile_ = CreateFile(StringUtils::format(L"\\\\.\\%c:", driver).c_str(),
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
		OPEN_EXISTING, 0, 0);
	if (hFile_ == INVALID_HANDLE_VALUE)
		return false;

	std::vector<char> buffer;
	getBuffer(0, 512, &buffer);
	if (memcmp(buffer.data() + 3, "NTFS", 4) != 0)
		return false;
	dbr_ = *reinterpret_cast<DBR*>((char*)buffer.data());
	return true;
}

bool NTFSParser::getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos)
{
	if (dir.empty() || !fileInfos)
		return false;

	FileRecord* fr = getFR(5);
	if (!fr)
		return false;
	auto index = dir.find_first_of(L"\\");
	if (index == -1)  {
		return fr->getFilesInDir(L"", fileInfos);
	} else {
		return fr->getFilesInDir(dir.substr(index + 1), fileInfos);
	}
}

bool NTFSParser::getBuffer(uint64_t offset, uint32_t size, std::vector<char>* buffer)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile_, li.LowPart, &li.HighPart, FILE_BEGIN)) {
		return false;
	}
	size_t oldSize = buffer->size();
	buffer->resize(oldSize + size);
	auto ptr = buffer->data() + oldSize;
	DWORD bytesRead;
	if (!ReadFile(hFile_, ptr, size, &bytesRead, NULL) || bytesRead != size)
		return false;
	return true;
}

bool NTFSParser::getFRBuffer(uint64_t frNumber, std::vector<char>* buffer)
{	
	uint64_t offset = 0;
	if (frNumber < 16) {
		offset = dbr_.mftStartLCN * dbr_.bytesPerCluster() + frNumber * dbr_.bytesPerFR();
		return getBuffer(offset, dbr_.bytesPerFR(), buffer);
	} else {
		FileRecord* mft = getFR(0);
		return mft->getData(frNumber * dbr_.bytesPerFR(), dbr_.bytesPerFR(), buffer);
	}
}

DBR& NTFSParser::dbr()
{
	return dbr_;
}

FileRecord* NTFSParser::getFR(uint64_t frNumber)
{
	if (!frs_.count(frNumber)) {
		FileRecord* fr = new FileRecord(this);
		if (!fr->parse(frNumber)) {
			delete fr;
			return nullptr;
		}
		frs_[frNumber] = fr;
	}
	return frs_[frNumber];
}
