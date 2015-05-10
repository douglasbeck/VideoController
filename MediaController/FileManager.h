#pragma once

using namespace System;

ref class FileManager
{
public:

	FileManager(String^ baseName);

	String^ GetNextFile();
	~FileManager();


private:

	String^ baseFileName;
};

