#pragma once

using namespace System;
using namespace System::Threading::Tasks;

ref class FileManager
{
public:

	FileManager(String^ baseName, int fileQuota);

	String^ GetNextFile();
	~FileManager();


private:

    void PruneFileList();
    int     maxFileQuota;
	String^ baseFileName;
    Task^   filePruner;
};

