#pragma once

class StringUtils
{
public:
	StringUtils();
	~StringUtils();

	static std::string format(const char* format, ...);


	static std::wstring format(const wchar_t* format, ...);


};

