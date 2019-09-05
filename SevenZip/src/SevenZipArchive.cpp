#include "stdafx.h"

#include "SevenZipArchive.h"
#include "UsefulFunctions.h"

using namespace SevenZipInt;


SevenZipArchive::SevenZipArchive(const SevenZipLibraryManager &libraryManager, const std::wstring &archivePath)
	: _ReadMetadata(false)
	, _OverrideCompressionFormat(false)
	, _libraryManager(libraryManager)
	, _archivePath(archivePath)
	, _compressionFormat(CompressionFormat::SevenZip)
	, _compressionLevel(CompressionLevel::Normal)
	, _numberOfItems(0)
{
}


SevenZipArchive::~SevenZipArchive()
{
	_itemNames.ClearAndFree();
	_orgSizes.ClearAndFree();
}

void SevenZipArchive::SetCompressionFormat(const CompressionFormat &format)
{
	_OverrideCompressionFormat = true;
	_ReadMetadata = false;
	_compressionFormat = format;
}

void SevenZipArchive::SetCompressionLevel(const CompressionLevel &level)
{
	_compressionLevel = level;
}

CompressionLevel SevenZipArchive::GetCompressionLevel()
{
	return _compressionLevel;
}

CompressionFormat SevenZipArchive::GetCompressionFormat()
{
	if (!_ReadMetadata && !_OverrideCompressionFormat)
		_ReadMetadata = ReadInArchiveMetadata();

	return _compressionFormat;
}

size_t SevenZipArchive::GetNumberOfItems()
{
	if (!_ReadMetadata)
		_ReadMetadata = ReadInArchiveMetadata();

	return _numberOfItems;
}

CObjectVector<std::wstring> SevenZipArchive::GetItemsNames()
{
	if (!_ReadMetadata)
		_ReadMetadata = ReadInArchiveMetadata();
	
	return _itemNames;
}

CObjectVector<size_t> SevenZipArchive::GetOrigSizes()
{
	if (!_ReadMetadata)
		_ReadMetadata = ReadInArchiveMetadata();

	return _orgSizes;
}

// Sets up all the metadata for an archive file
bool SevenZipArchive::ReadInArchiveMetadata()
{
	bool DetectedCompressionFormat = true;
	if (!_OverrideCompressionFormat)
	{
		DetectedCompressionFormat = DetectCompressionFormat();
	}
	bool GotItemNumberNamesAndOrigSizes = GetItemsNamesInt();

	return (DetectedCompressionFormat && GotItemNumberNamesAndOrigSizes);
}

bool SevenZipArchive::DetectCompressionFormat()
{
	_OverrideCompressionFormat = false;
	return DetectCompressionFormat();
}

bool SevenZipArchive::DetectCompressionFormatInt()
{
	_OverrideCompressionFormat = false;
	return DetectCompressionFormat(_compressionFormat);
}

bool SevenZipArchive::DetectCompressionFormat(CompressionFormat &format)
{
	_OverrideCompressionFormat = false;
	
	return UsefulFunctions::DetectCompressionFormat(_libraryManager, _archivePath, format, _password);
}

bool SevenZipArchive::GetNumberOfItemsInt()
{
	return UsefulFunctions::GetNumberOfItems(_libraryManager, _archivePath,
		_compressionFormat, _numberOfItems, _password);
}

bool SevenZipArchive::GetItemsNamesInt()
{
	return UsefulFunctions::GetItemsNames(_libraryManager, _archivePath, _compressionFormat,
		_numberOfItems, _itemNames, _orgSizes, _password);
}

void SevenZipArchive::SetPassword(const std::wstring& password)
{
	_password = password;
}
