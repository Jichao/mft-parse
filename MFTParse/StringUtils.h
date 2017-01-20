#pragma once

class StringUtils
{
public:
	StringUtils();
	~StringUtils();

	static std::string format(const char* format, ...);
	static std::wstring format(const wchar_t* format, ...);

	static std::wstring StrToUTF16(int encoding, const std::string& str);
	static std::string UTF16ToStr(int encoding, const std::wstring& str);
	static std::wstring ANSIToUTF16(const std::string& str);
	static std::string UTF16ToANSI(const std::wstring& str);
};

