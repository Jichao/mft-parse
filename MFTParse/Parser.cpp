#include "stdafx.h"
#include "Parser.h"
#include "StringUtils.h"
#include "FileRecord.h"

std::map<wchar_t, Parser*> Parser::s_parsers;

Parser* Parser::GetParserForDriver(wchar_t driver)
{
	if (!s_parsers.count(driver)) {
		Parser* parser = new Parser();
		if (!parser->init(driver)) {
			delete parser;
			return nullptr;
		} else {
			s_parsers[driver] = parser;
		}
	} 
	return s_parsers[driver];
}

Parser::Parser()
{
}


Parser::~Parser()
{
}

bool Parser::init(wchar_t driver)
{
	hFile_ = CreateFile(StringUtils::format(L"\\\\.\\%c:", driver).c_str(),
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
		OPEN_EXISTING, 0, 0);
	if (hFile_ == INVALID_HANDLE_VALUE)
		return false;

	std::vector<char> buffer;
	getBuffer(0, 512, &buffer);
	if (memcmp(buffer.data(), "NTFS", 4) != 0)
		return false;
	dbr_ = *reinterpret_cast<DBR*>((char*)buffer.data() + 4);
	return true;
}

bool Parser::getFilesInDir(const std::wstring& dir, std::vector<FileInfo>* fileInfos)
{
	if (dir.empty() || !fileInfos)
		return false;

	uint64_t bootOffset = dbr_.mftStartLCN * dbr_.bytesPerCluster() + 5 * dbr_.bytesPerFR();
	FileRecord* fr = new FileRecord(this);
	if (!fr->parse(bootOffset)) {
		delete fr;
		return false;
	}
	rootFR_.reset(fr);
	return rootFR_->getFilesInDir(dir.substr(dir.find_first_of(L":") + 1), fileInfos);
}


bool Parser::getBuffer(uint64_t offset, uint32_t size, std::vector<char>* buffer)
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


DBR& Parser::dbr()
{
	return dbr_;
}

