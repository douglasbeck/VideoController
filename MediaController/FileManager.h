#pragma once

#include <windows.h>
#include <string>

using namespace std;

class FileManager
{
public:

	FileManager(PCWSTR baseName);

	wstring GetNextFile();
	~FileManager();


private:

	wstring baseFileName;

	static void CALLBACK CycleTimerCallback(PVOID windowHandle, BOOLEAN timerOrWaitFired);
};

