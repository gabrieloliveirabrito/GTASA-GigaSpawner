#pragma once // For Visual Studio
#ifndef INIREADER_H
#define INIREADER_H

#include <stdlib.h>
#include <iostream>
#include <Windows.h>

class CIniReader
{
public:
	CIniReader(const char* szFileName);
	CIniReader(std::string szFileName);

	int ReadInteger(const char* szSection, const char* szKey, int iDefaultValue);
	float ReadFloat(const char* szSection, const char* szKey, float fltDefaultValue);
	bool ReadBoolean(const char* szSection, const char* szKey, bool bolDefaultValue);
	char* ReadString(const char* szSection, const char* szKey, const char* szDefaultValue);
private:
	char m_szFileName[255];
};
#endif//INIREADER_H