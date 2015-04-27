#pragma once

#include <windows.h>
#include <string>

using namespace std;

class FileManager
{
public:

	FileManager(PCWSTR baseName, HWND mainWindow);

	wstring GetNextFile();

	void StartCycleTimer();
	void StopCycleTimer();
	void OnRecordingStopped();

	~FileManager();


private:

	HWND	hwndMainWindow;
	wstring baseFileName;
	HANDLE  timerHandle;

	static void CALLBACK CycleTimerCallback(PVOID windowHandle, BOOLEAN timerOrWaitFired);
};

