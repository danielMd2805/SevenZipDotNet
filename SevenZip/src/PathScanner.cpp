#include "stdafx.h"

#include "PathScanner.h"
#include "FileSys.h"

#include <deque>

using namespace SevenZipInt;


void PathScanner::Scan(const std::wstring &root, const std::wstring &pathPrefix, Callback &cb)
{
	Scan(root, pathPrefix, _T("*"), cb);
}

void PathScanner::Scan(const std::wstring &root, const std::wstring &pathPrefix, const std::wstring &searchPattern, Callback &cb)
{
	std::deque< std::wstring > directories;
	directories.push_back(root);

	while (!directories.empty())
	{
		std::wstring directory = directories.front();
		directories.pop_front();

		if (ExamineFiles(directory, searchPattern, pathPrefix, cb))
			break;

		ExamineDirectories(directory, directories, pathPrefix, cb);
	}
}

bool PathScanner::ExamineFiles(const std::wstring &directory, const std::wstring &searchPattern, const std::wstring &pathPrefix, Callback &cb)
{
	std::wstring findStr = FileSys::AppendPath(directory, searchPattern);
	bool exit = false;

	WIN32_FIND_DATA fdata;
	const HANDLE hFile = FindFirstFile(findStr.c_str(), &fdata);
	
	if (hFile == INVALID_HANDLE_VALUE)
		return exit;

	cb.EnterDirectory(directory);

	do
	{
		const SevenZip::FilePathInfo fpInfo = ConvertFindInfo(directory, pathPrefix, fdata);
		if (!fpInfo.IsDirectory && !IsSpecialFileName(fpInfo.FileName))
			cb.ExamineFile(fpInfo, exit);

	} while (!exit && FindNextFile(hFile, &fdata));

	if (!exit)
		cb.LeaveDirectory(directory);

	FindClose(hFile);
	return exit;
}

void PathScanner::ExamineDirectories(const std::wstring &directory, std::deque< std::wstring > &subDirs, const std::wstring &pathPrefix, Callback &cb)
{
	std::wstring findStr = FileSys::AppendPath(directory, _T("*"));

	WIN32_FIND_DATA fdata;
	HANDLE hFile = FindFirstFile(findStr.c_str(), &fdata);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	do
	{
		SevenZip::FilePathInfo fpInfo = ConvertFindInfo(directory, pathPrefix, fdata);
		if (fpInfo.IsDirectory && !IsSpecialFileName(fpInfo.FileName) && cb.ShouldDescend(fpInfo))
			subDirs.push_back(fpInfo.FilePath);

	} while (FindNextFile(hFile, &fdata));

	FindClose(hFile);
}

bool PathScanner::IsAllFilesPattern(const std::wstring &searchPattern)
{
	return searchPattern == _T("*") || searchPattern == _T("*.*");
}

bool PathScanner::IsSpecialFileName(const std::wstring &fileName)
{
	return fileName == _T(".") || fileName == _T("..");
}

bool PathScanner::IsDirectory(const WIN32_FIND_DATA &fdata)
{
	return (fdata.dwFileAttributes  &FILE_ATTRIBUTE_DIRECTORY) != 0;
}

SevenZip::FilePathInfo PathScanner::ConvertFindInfo(const std::wstring &directory, const std::wstring &pathPrefix, const WIN32_FIND_DATA &fdata)
{
	SevenZip::FilePathInfo file;
	file.memFile = false;
	file.rootPath = pathPrefix;
	file.FileName = fdata.cFileName;
	file.FilePath = FileSys::AppendPath(directory, file.FileName);
	file.LastWriteTime = fdata.ftLastWriteTime;
	file.CreationTime = fdata.ftCreationTime;
	file.LastAccessTime = fdata.ftLastAccessTime;
	file.Attributes = fdata.dwFileAttributes;
	file.IsDirectory = IsDirectory(fdata);

	ULARGE_INTEGER size;
	size.LowPart = fdata.nFileSizeLow;
	size.HighPart = fdata.nFileSizeHigh;
	file.Size = size.QuadPart;

	return file;
}