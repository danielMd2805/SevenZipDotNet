#pragma once

#include "stdafx.h"

#include "FileInfo.h"
#include "FileStreams.h"

namespace SevenZipInt
{
	class FileSys
	{
	public:
		static std::wstring GetPath(const std::wstring &filePath);
		static std::wstring GetFileName(const std::wstring &filePathOrName);
		static std::wstring AppendPath(const std::wstring &left, const std::wstring &right);
		static std::wstring ExtractRelativePath(const std::wstring &basePath, const std::wstring &fullPath);

		static bool DirectoryExists(const std::wstring &path);
		static bool IsDirectoryEmptyRecursive(const std::wstring &path);

		static bool CreateDirectoryTree(const std::wstring &path);

		static CObjectVector<SevenZip::FilePathInfo> GetFile(const std::wstring &filePathOrName, bool absolutePath = false);
		static CObjectVector<SevenZip::FilePathInfo> GetFilesInDirectory(const std::wstring &directory, const std::wstring &searchPattern, const std::wstring &pathPrefix, bool recursive);

		static CComPtr<CInFileStream> OpenFileToRead(const std::wstring &filePath);
		static CComPtr<COutFileStream> OpenFileToWrite(const std::wstring &filePath);

		static bool MoveFile(const std::wstring &sourcePath, const std::wstring &targetPath);
	};

}
