#pragma once

#include <windows.h>
#include <string>


namespace SevenZip
{
	struct FileInfo
	{
		std::wstring		FileName;
		FILETIME	LastWriteTime = { 0 };
		FILETIME	CreationTime = { 0 };
		FILETIME	LastAccessTime = { 0 };
		ULONGLONG	Size = 0ULL;
		ULONGLONG	PackedSize = 0ULL;
		UINT		Attributes = 0;
		bool		IsDirectory = false;
		bool		memFile = false;
		void*		memPointer = nullptr;
	};

	struct FilePathInfo : public FileInfo
	{
		std::wstring		rootPath;
		std::wstring		FilePath;
	};
}

