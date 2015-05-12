#include "stdafx.h"
#include "FileManager.h"


using namespace System::IO;
using namespace System::Text;
using namespace System::Threading::Tasks;

FileManager::FileManager(String^ baseName, int fileQuota)
{
    baseFileName = gcnew String(baseName);
    fileQuota < 4 ? maxFileQuota = 4 : maxFileQuota = fileQuota;
}

String^ FileManager::GetNextFile()
{
    DateTime dateTime = DateTime::Now;

    Task pruner(gcnew Action(this, &FileManager::PruneFileList));
    pruner.Start();

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

void FileManager::PruneFileList()
{
    String^ path;
    StringBuilder^ searchPattern = gcnew StringBuilder();

    int pathSepPosition = baseFileName->LastIndexOf("\\");

    path = baseFileName->Substring(0, pathSepPosition + 1);
    searchPattern->Append(baseFileName->Substring(pathSepPosition + 1));
    searchPattern->Append("*");

    DirectoryInfo^ dirInfo = gcnew DirectoryInfo(path);

    array<FileInfo^>^ files = dirInfo->GetFiles(searchPattern->ToString());
}

FileManager::~FileManager()
{
}

