#include "stdafx.h"
#include "FileManager.h"

using namespace System::Text;

FileManager::FileManager(String^ baseName)
{
    baseFileName = gcnew String(baseName);
}

String^ FileManager::GetNextFile()
{
    DateTime dateTime = DateTime::Now;

	StringBuilder^ fileName = gcnew StringBuilder(baseFileName);

    fileName->Append(L"-");
    fileName->Append(dateTime.Year);
    fileName->Append(L"-");
    fileName->Append(dateTime.Month);
    fileName->Append(L"-");
    fileName->Append(dateTime.Day);
    fileName->Append(L"-");
    fileName->Append(dateTime.Hour);
    fileName->Append(L"-");
    fileName->Append(dateTime.Minute);
    fileName->Append(L"-");
    fileName->Append(dateTime.Second);
    fileName->Append(L".mp4");

	return fileName->ToString();
}

FileManager::~FileManager()
{
}

