#include "stdafx.h"
#include "StringUtils.h"


StringUtils::StringUtils()
{
}


StringUtils::~StringUtils()
{
}

std::wstring StringUtils::format(const wchar_t* format, ...)
{
	if (!format) {
		return L"";
	}

	std::vector<wchar_t> buff;
	size_t size = wcslen(format) * 2;
	buff.resize(size);
	va_list ap;
	va_start(ap, format);
	int ret = -1;
	while (true) {
		ret = _vsnwprintf_s(buff.data(), size, _TRUNCATE, format, ap);
		if (ret != -1)
			break;
		else {
			size *= 2;
			buff.resize(size);
		}
	}
	va_end(ap);
	if (ret == -1)
		return L"";
	else
		return std::wstring(buff.data(), ret);
}

std::string StringUtils::format(const char* format, ...)
{
	if (!format) {
		return "";
	}

	std::vector<char> buff;
	size_t size = strlen(format) * 2;
	buff.resize(size);
	va_list ap;
	va_start(ap, format);
	int ret;
	while (true) {
		ret = _vsnprintf_s(buff.data(), size, _TRUNCATE, format, ap);
		if (ret != -1)
			break;
		else {
			size *= 2;
			buff.resize(size);
		}
	}
	va_end(ap);
	if (ret == -1)
		return "";
	else
		return std::string(buff.data(), ret);
}
