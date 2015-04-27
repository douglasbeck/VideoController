#include "stdafx.h"
#include "FileManager.h"
#include "resource.h"
#include <ctime>
#include <iostream>

//VOID DbgPrint(PCTSTR format, ...);

FileManager::FileManager(PCWSTR baseName, HWND mainWindow)
{
	hwndMainWindow = mainWindow;
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

void FileManager::StopCycleTimer()
{
	//DbgPrint(L"FileManager::StopCycleTimer");

	DeleteTimerQueueTimer( NULL, timerHandle, NULL);
}

void FileManager::StartCycleTimer()
{
	//DbgPrint(L"FileManager::StartCycleTimer");

	BOOL timerCreated = ::CreateTimerQueueTimer(
		&timerHandle,
		NULL,
		CycleTimerCallback,
		(PVOID)(hwndMainWindow),
		900000,
		900000,
		WT_EXECUTEDEFAULT);
}

void FileManager::OnRecordingStopped()
{
	//DbgPrint(L"FileManager::OnRecordingStopped");

	//BOOL bReturn = PostMessage(hwndMainWindow, WM_COMMAND, ID_AUTO_CAPTURE_START, 0);

	//if (!bReturn)
	//{
	//	cout << "Fail";
	//}
}

void CALLBACK FileManager::CycleTimerCallback(PVOID windowHandle, BOOLEAN timerOrWaitFired)
{
	//DbgPrint(L"FileManager::CycleTimerCallback");

	//BOOL bReturn = PostMessage((HWND)windowHandle, WM_COMMAND, ID_AUTO_CAPTURE_STOP, 0);

	//if (!bReturn)
	//{
	//	cout << "Fail";
	//}
}

FileManager::~FileManager()
{
}

