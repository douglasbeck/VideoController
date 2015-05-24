#include "stdafx.h"
#include "FileManager.h"

using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Text;
using namespace System::Threading;

ref class FileInfoComparer : IComparer
{
public:

    virtual int Compare(Object^ x, Object^ y);
};

int FileInfoComparer::Compare(Object^ x, Object^ y)
{
    FileInfo^ fx = static_cast<FileInfo^>(x);
    FileInfo^ fy = static_cast<FileInfo^>(y);

    if (fx->CreationTimeUtc < fy->CreationTimeUtc)
    {
        return -1;
    }
    else if (fx->CreationTimeUtc > fy->CreationTimeUtc)
    {
        return 1;
    }

    return 0;
}

FileManager::FileManager(String^ baseName, int fileQuota)
{
    baseFileName = gcnew String(baseName);
    fileQuota < 4 ? maxFileQuota = 4 : maxFileQuota = fileQuota;
}

String^ FileManager::GetNextFile()
{
    DateTime dateTime = DateTime::Now;

    filePruner = gcnew Task(gcnew Action(this, &FileManager::PruneFileList));
    filePruner->Start();

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
    auto sw = gcnew Stopwatch();
    sw->Start();
 
    String^ path;
    StringBuilder^ searchPattern = gcnew StringBuilder();

    int pathSepPosition = baseFileName->LastIndexOf("\\");

    path = baseFileName->Substring(0, pathSepPosition + 1);
    searchPattern->Append(baseFileName->Substring(pathSepPosition + 1));
    searchPattern->Append("*");

    DirectoryInfo^ dirInfo = gcnew DirectoryInfo(path);

    array<FileInfo^>^ files = dirInfo->GetFiles(searchPattern->ToString());

    if (files->Length > maxFileQuota)
    {
        Debug::Print("PruneFileList:: Number of files = {0}", files->Length);

        array<FileInfo^>::Sort(files, gcnew FileInfoComparer());

        for (int i = 0; i <= files->Length - maxFileQuota - 1; i++)
        {
            Debug::Print("PruneFileList:: Deleting {0}", files[i]->Name);

            files[i]->Delete();
        }
    }

    Debug::Print("PruneFileList: Thread {0} Time {1}", Thread::CurrentThread->ManagedThreadId, sw->ElapsedTicks);
}

FileManager::~FileManager()
{
}

