#include "stdafx.h"

#include <7z/CPP/7zip/Archive/IArchive.h>

#include "FileSys.h"
#include "FileStreams.h"
#include "PropVariant.h"
#include "UsefulFunctions.h"
#include "ProgressCallback.h"
#include "OutStreamWrapper.h"
#include "SevenZipCompressor.h"
#include "ArchiveUpdateCallback.h"


#include <atltime.h>

using namespace SevenZip;
using namespace SevenZipInt;

const std::wstring SearchPatternAllFiles = _T("*");

SevenZipCompressor::SevenZipCompressor(const SevenZipLibraryManager &library, const std::wstring &archivePath, bool useTempArchive)
	: SevenZipArchive(library, archivePath)
	, _absolutePath(false)
	, _useTempArchive(useTempArchive)
{
}

bool SevenZipCompressor::AddDirectory(const std::wstring &directory, bool includeSubdirs /*= true*/)
{
	return AddFilesToList(directory, SearchPatternAllFiles, _absolutePath ? _T("") : FileSys::GetPath(directory), includeSubdirs);
}

bool SevenZipCompressor::AddFiles(const std::wstring &directory, const std::wstring &searchFilter, bool includeSubdirs /*= true*/)
{
	return AddFilesToList(directory, searchFilter, _absolutePath ? _T("") : directory, includeSubdirs);
}

bool SevenZipCompressor::AddAllFiles(const std::wstring &directory, bool includeSubdirs /*= true*/)
{
	return AddFilesToList(directory, SearchPatternAllFiles, _absolutePath ? _T("") : directory, includeSubdirs);
}

bool SevenZipCompressor::AddMemory(const std::wstring &filePath, void* memPointer, size_t size)
{
	FilePathInfo memFile;
	memFile.rootPath = FileSys::GetPath(filePath);
	memFile.FileName = FileSys::GetFileName(filePath);
	memFile.memFile = true;
	memFile.memPointer = memPointer;
	memFile.Size = size;
	memFile.CreationTime = memFile.LastAccessTime = memFile.LastWriteTime = CFileTime::GetCurrentTime();
	memFile.IsDirectory = false;
	memFile.Attributes = FILE_ATTRIBUTE_NORMAL;

	_fileList.Add(memFile);

	return true;
}

bool SevenZipCompressor::AddFile(const std::wstring &filePath)
{
	CObjectVector< FilePathInfo > files = FileSys::GetFile(filePath, _absolutePath);

	if (files.IsEmpty())
	{
		return false;
	}

	for (int i = 0; i < files.Size(); i++)
	{
		_fileList.Add(files[i]);
	}

	return true;
}

bool SevenZipCompressor::DoCompress(ProgressCallback* callback /*= nullptr*/)
{
	if (_fileList.IsEmpty())
	{
		return false;
	}

	if (!CheckValidFormat())
	{
		return false;
	}

	CComPtr< IOutArchive > archiver = UsefulFunctions::GetArchiveWriter(_libraryManager, _compressionFormat);
	if (!archiver)
	{
		// compression not supported
		return false;
	}

	SetCompressionProperties(archiver);
	
	if (_endingWithCompressionFormat)
	{
		//Set full outputFilePath including ending
		_archivePath += UsefulFunctions::EndingFromCompressionFormat(_compressionFormat);
	}

	if (_useTempArchive)
		_archivePath += _T(".tmp");

	const CComPtr<OutStreamWrapper> outFile = new OutStreamWrapper(OpenArchiveStream());
	const CComPtr<ArchiveUpdateCallback> updateCallback = new ArchiveUpdateCallback(_fileList, _archivePath, _password, callback);

	HRESULT hr = archiver->UpdateItems(outFile, static_cast<UInt32>(_fileList.Size()), updateCallback);

	if (callback)
	{
		callback->OnDone(_archivePath);	//Todo: give full path support
	}

	outFile->CloseStream();

	if (_useTempArchive)
	{
		if (!FileSys::MoveFile(_archivePath, _archivePath.substr(0, _archivePath.length() - 4)))
		{
			return false;
		}

	}

	// returning S_FALSE also indicates error
	return (hr == S_OK) ? true : false;
}

bool SevenZipCompressor::CheckValidFormat() const
{
	if (_fileList.Size() > 1 &&
		(_compressionFormat == SevenZip::CompressionFormat::BZip2
			|| _compressionFormat == SevenZip::CompressionFormat::GZip)
		)
	{
		// Not supported by compressing format
		return false;
	}

	return true;
}

CComPtr< COutFileStream > SevenZipCompressor::OpenArchiveStream() const
{
	return FileSys::OpenFileToWrite(_archivePath);
}

bool SevenZipCompressor::AddFilesToList(const std::wstring &directory, const std::wstring &searchPattern, const std::wstring &pathPrefix, bool recursion)
{
	if (!FileSys::DirectoryExists(directory))
	{
		return false;	//Directory does not exist
	}

	if (FileSys::IsDirectoryEmptyRecursive(directory))
	{
		return false;	//Directory \"%s\" is empty" ), directory.c_str()
	}

	CObjectVector<FilePathInfo> files = FileSys::GetFilesInDirectory(directory, searchPattern, pathPrefix, recursion);
	if (files.IsEmpty())
	{
		return false;
	}

	for (int i = 0; i < files.Size(); i++)
	{
		_fileList.Add(files[i]);
	}

	return true;
}

bool SevenZipCompressor::SetCompressionProperties(IUnknown* outArchive) const
{
	if (!outArchive)
	{
		return false;
	}

	const size_t numProps = 1;
	const wchar_t* names[numProps] = { L"x" };
	CPropVariant values[numProps] = { static_cast<UInt32>(_compressionLevel) };

	CComPtr< ISetProperties > setter;
	outArchive->QueryInterface(IID_ISetProperties, reinterpret_cast<void**>(&setter));
	if (setter == nullptr)
	{
		return false;	//Archive does not support setting compression properties
	}

	HRESULT hr = setter->SetProperties(names, values, numProps);

	// returning S_FALSE also indicates error
	return hr == S_OK ? true : false;
}
