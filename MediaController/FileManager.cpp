#include "stdafx.h"
#include "FileManager.h"
#include "resource.h"
#include <ctime>
#include <iostream>

//VOID DbgPrint(PCTSTR format, ...);

FileManager::FileManager(PCWSTR baseName)
{
	baseFileName.append(baseName);
}

wstring FileManager::GetNextFile()
{
	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);

	cout << (now->tm_year + 1900) << '-'
		<< (now->tm_mon + 1) << '-'
		<< now->tm_mday
		<< endl;

	wstring fileName = baseFileName;

	fileName.append(L"-");

	fileName.append(to_wstring(now->tm_year+1900));
	fileName.append(L"-");
	fileName.append(to_wstring(now->tm_mon+1));
	fileName.append(L"-");
	fileName.append(to_wstring(now->tm_mday));
	fileName.append(L"-");
	fileName.append(to_wstring(now->tm_hour));
	fileName.append(L"-");
	fileName.append(to_wstring(now->tm_min));
	fileName.append(L"-");
	fileName.append(to_wstring(now->tm_sec));
	fileName.append(L".mp4");

	return fileName;
}

FileManager::~FileManager()
{
}

