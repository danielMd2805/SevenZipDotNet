#include "stdafx.h"

#include "FileSys.h"
#include "PathScanner.h"

#include <ShlObj_core.h>
#include <iostream>


using namespace SevenZipInt;


class ScannerCallback : public PathScanner::Callback
{
private:

	CObjectVector< SevenZip::FilePathInfo > m_files;
	bool m_recursive;
	bool m_onlyFirst;

public:

	explicit ScannerCallback(bool recursive, bool onlyFirst = false) : m_recursive(recursive), m_onlyFirst(onlyFirst) {}
	const CObjectVector< SevenZip::FilePathInfo >& GetFiles() const { return m_files; }

	bool ShouldDescend(const SevenZip::FilePathInfo& directory) override { return m_recursive; }

	void ExamineFile(const SevenZip::FilePathInfo& file, bool& exit) override
	{
		m_files.Add(file);
		if (m_onlyFirst)
		{
			exit = true;
		}
	}
};

class IsEmptyCallback : public PathScanner::Callback
{
private:

	bool m_isEmpty;

public:

	IsEmptyCallback() : m_isEmpty(true) {}
	bool IsEmpty() const { return m_isEmpty; }

	bool ShouldDescend(const SevenZip::FilePathInfo& directory) override { return true; }

	void ExamineFile(const SevenZip::FilePathInfo& file, bool& exit) override
	{ m_isEmpty = false; exit = true; }
};

std::wstring FileSys::GetPath(const std::wstring& filePath)
{
	// Find the last "\" or "/" in the string and return it and everything before it.
	size_t index = filePath.rfind(_T('\\'));
	size_t index2 = filePath.rfind(_T('/'));

	if (index2 != std::string::npos && index2 > index)
		index = index2;

	if (index == std::string::npos)
	{
		// No path sep.
		return std::wstring();
	}
	
	if (index + 1 >= filePath.size())
	{
		// Last char is path sep, the whole thing is a path.
		return filePath;
	}
	
	return filePath.substr(0, index + 1);
}

std::wstring FileSys::GetFileName(const std::wstring& filePathOrName)
{
	// Find the last "\" or "/" in the string and return everything after it.
	size_t index = filePathOrName.rfind(_T('\\'));
	size_t index2 = filePathOrName.rfind(_T('/'));

	if (index2 != std::string::npos && index2 > index)
		index = index2;

	if (index == std::string::npos)
	{
		// No path sep, return the whole thing.
		return filePathOrName;
	}
	
	if (index + 1 >= filePathOrName.size())
	{
		// Last char is path sep, no filename.
		return std::wstring();
	}
	
	return filePathOrName.substr(index + 1, filePathOrName.size() - (index + 1));
}

std::wstring FileSys::AppendPath(const std::wstring& left, const std::wstring& right)
{
	if (left.empty())
		return right;

	TCHAR lastChar = left[left.size() - 1];
	if (lastChar == _T('\\') || lastChar == _T('/'))
		return left + right;
	
	return left + _T("\\") + right;
}

std::wstring FileSys::ExtractRelativePath(const std::wstring& basePath, const std::wstring& fullPath)
{
	if (basePath.size() >= fullPath.size())
		return std::wstring();

	if (basePath != fullPath.substr(0, basePath.size()))
		return std::wstring();

	return fullPath.substr(basePath.size(), fullPath.size() - basePath.size());
}

bool FileSys::DirectoryExists(const std::wstring& path)
{
	DWORD attributes = GetFileAttributes(path.c_str());

	if (attributes == INVALID_FILE_ATTRIBUTES)
		return false;
	
	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool FileSys::IsDirectoryEmptyRecursive(const std::wstring& path)
{
	IsEmptyCallback cb;
	PathScanner::Scan(path, _T(""), cb);
	return cb.IsEmpty();
}

bool FileSys::CreateDirectoryTree(const std::wstring& path)
{
	int ret = SHCreateDirectoryEx(NULL, path.c_str(), NULL);
	return ret == ERROR_SUCCESS;
}

CObjectVector<SevenZip::FilePathInfo> FileSys::GetFile(const std::wstring& filePathOrName, bool absolutePath /*= false*/)
{
	std::wstring path = FileSys::GetPath(filePathOrName);
	std::wstring name = FileSys::GetFileName(filePathOrName);

	ScannerCallback cb(false, true);
	PathScanner::Scan(path, absolutePath ? _T("") : path, name, cb);
	return cb.GetFiles();
}

CObjectVector<SevenZip::FilePathInfo> FileSys::GetFilesInDirectory(const std::wstring& directory, const std::wstring& searchPattern, const std::wstring& pathPrefix, bool recursive)
{
	ScannerCallback cb(recursive);
	PathScanner::Scan(directory, pathPrefix, searchPattern, cb);
	return cb.GetFiles();
}

CComPtr< CInFileStream > FileSys::OpenFileToRead(const std::wstring& filePath)
{
	CInFileStream *inStreamSpec = new CInFileStream();
	const WCHAR* filePathStr = filePath.c_str();

	if (inStreamSpec->Open(filePathStr))
		return inStreamSpec;
	return nullptr;
	/*CComPtr< IStream > fileStream;

	const WCHAR* filePathStr = filePath.c_str();
	if (FAILED(SHCreateStreamOnFileEx(filePathStr, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &fileStream)))
		return NULL;

	return fileStream;*/
}

CComPtr< COutFileStream > FileSys::OpenFileToWrite(const std::wstring& filePath)
{
	//Will use FileStream by Igor Pavlov but in first test case not faster then IStream
	COutFileStream *outStreamSpec = new COutFileStream();
	const WCHAR* filePathStr = filePath.c_str();

	if (outStreamSpec->Create(filePathStr, true))
		return outStreamSpec;

	if (::GetLastError() != ERROR_FILE_EXISTS)
		return nullptr;

	return nullptr;

	/*CComPtr< IStream > fileStream;
	

	const WCHAR* filePathStr = filePath.c_str();
	if (FAILED(SHCreateStreamOnFileEx(filePathStr, STGM_CREATE | STGM_WRITE, FILE_ATTRIBUTE_NORMAL, TRUE, NULL, &fileStream)))
		return nullptr;

	return fileStream;*/
}

bool FileSys::MoveFile(const std::wstring& sourcePath, const std::wstring& targetPath)
{
	const WCHAR* wSource = sourcePath.c_str();
	const WCHAR* wTarget = targetPath.c_str();

	if (::MoveFileExW(wSource, wTarget, MOVEFILE_REPLACE_EXISTING) == FALSE)
	{
		int errorCode = ::GetLastError();
		std::wstring msg = _T("An error occoured while moving file. Code: ");
		msg.append(std::to_wstring(errorCode));
		std::wcout << msg << std::endl;
		return false;
	}
	return true;
}

