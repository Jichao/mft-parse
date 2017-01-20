// MFTParse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NTFSParser.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1) {
		printf("listfiles: dir-path\n");
		return -1;
	}
	std::wstring dir = argv[1];
	NTFSParser* parser = NTFSParser::GetParserForDriver(dir[0]);
	std::vector<FileInfo> files;
	if (!parser) {
		return -1;
	}
	parser->getFilesInDir(dir, &files);
	for (auto fi : files) {
		printf("%S\n", fi.name.c_str());
	}
	return 0;
}

