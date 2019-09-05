#include "stdafx.h"
#include "SevenZipExtractor.h"
#include "GUIDs.h"
#include "FileSys.h"
#include "ArchiveOpenCallback.h"
#include "ArchiveExtractCallback.h"
#include "MemExtractCallback.h"
#include "InStreamWrapper.h"
#include "UsefulFunctions.h"


using namespace SevenZip;
using namespace SevenZipInt;

SevenZipExtractor::SevenZipExtractor(const SevenZipLibraryManager &library, const std::wstring &archivePath)
	: SevenZipArchive(library, archivePath)
{
}

SevenZipExtractor::~SevenZipExtractor()
{
}

bool SevenZipExtractor::ExtractArchive(const std::wstring &destDirectory, ProgressCallback* callback /*= nullptr*/)
{
	const CComPtr< CInFileStream > archiveStream = FileSys::OpenFileToRead( _archivePath );

	if (archiveStream == nullptr)
	{
		return false;	//Could not open archive
	}

	return ExtractFilesFromArchive(archiveStream, NULL, -1, destDirectory, callback);
}

bool SevenZipExtractor::ExtractFilesFromArchive(const unsigned int* fileIndices,
												const unsigned int numberFiles,
												const std::wstring &destDirectory,
												ProgressCallback* callback /*= nullptr*/)
{
	const CComPtr< CInFileStream > archiveStream = FileSys::OpenFileToRead(_archivePath);

	if (archiveStream == nullptr)
	{
		return false;	//Could not open archive
	}

	return ExtractFilesFromArchive(archiveStream, fileIndices, numberFiles, destDirectory, callback);
}

bool SevenZipExtractor::ExtractFileToMemory(const unsigned int index, SevenZipMemoryQueue &bufferQueue, int bufferSize, ProgressCallback* callback /*= nullptr*/)
{
	const CComPtr< CInFileStream > archiveStream = FileSys::OpenFileToRead(_archivePath);
	if (archiveStream == nullptr)
	{
		return false;	//Could not open archive
	}

	CComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(_libraryManager, _compressionFormat);
	const CComPtr< InStreamWrapper > inFile = new InStreamWrapper(archiveStream);
	const CComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback(_password);

	HRESULT hr = archive->Open(inFile, nullptr, openCallback);
	if (hr != S_OK)
	{
		return false;	//Open archive error
	}

	const UInt32 indices[] = { index };

	CComPtr< MemExtractCallback > extractCallback = new MemExtractCallback(archive, bufferQueue, bufferSize, _archivePath, _password, callback);

	hr = archive->Extract(indices, 1, false, extractCallback);
	if (hr != S_OK)
	{
		// returning S_FALSE also indicates error
		return false;	//Extract archive error
	}

	if (callback)
	{
		callback->OnDone(_archivePath);
	}

	archive->Close();

	return true;

}

bool SevenZipExtractor::ExtractFilesFromArchive(const CComPtr<CInFileStream> &archiveStream,
												const unsigned int* filesIndices,
												const unsigned int numberFiles,
												const std::wstring &destDirectory,
												ProgressCallback* callback) const
{
	CComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader( _libraryManager, _compressionFormat );
	const CComPtr< InStreamWrapper > inFile = new InStreamWrapper( archiveStream );
	const CComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback(_password);

	HRESULT hr = archive->Open(inFile, 0, openCallback);
	if (hr != S_OK)
	{
		return false;	//Open archive error
	}
		

	const CComPtr< ArchiveExtractCallback > extractCallback = new ArchiveExtractCallback(archive, destDirectory, _archivePath, _compressionFormat,  _password, callback);

	hr = archive->Extract(filesIndices, numberFiles, false, extractCallback);
	if (hr != S_OK)
	{
		// returning S_FALSE also indicates error
		return false;	//Extract archive error
	}

	if (callback)
	{
		callback->OnDone(_archivePath);
	}

	archive->Close();

	return true;
}
