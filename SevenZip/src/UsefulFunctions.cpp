#include "stdafx.h"

#include "InStreamWrapper.h"
#include "ArchiveOpenCallback.h"
#include "FileSys.h"
#include "PropVariant.h"
#include "SevenZipLibraryManager.h"
#include "UsefulFunctions.h"

#include "GUIDs.h"

using namespace SevenZipInt;
using namespace SevenZip;

const GUID* UsefulFunctions::GetCompressionGUID(const CompressionFormat& format)
{
	const GUID* guid = nullptr;

	switch (format)
	{
	case CompressionFormat::Zip:
		guid = &SevenZipInt::CLSID_CFormatZip;
		break;

	case CompressionFormat::GZip:
		guid = &SevenZipInt::CLSID_CFormatGZip;
		break;

	case CompressionFormat::BZip2:
		guid = &SevenZipInt::CLSID_CFormatBZip2;
		break;

	case CompressionFormat::Rar:
		guid = &SevenZipInt::CLSID_CFormatRar;
		break;

	case CompressionFormat::Tar:
		guid = &SevenZipInt::CLSID_CFormatTar;
		break;

	case CompressionFormat::Iso:
		guid = &SevenZipInt::CLSID_CFormatIso;
		break;

	case CompressionFormat::Cab:
		guid = &SevenZipInt::CLSID_CFormatCab;
		break;

	case CompressionFormat::Lzma:
		guid = &SevenZipInt::CLSID_CFormatLzma;
		break;

	case CompressionFormat::Lzma86:
		guid = &SevenZipInt::CLSID_CFormatLzma86;
		break;

	default:
		guid = &SevenZipInt::CLSID_CFormat7z;
		break;
	}
	return guid;
}

CComPtr<IInArchive> UsefulFunctions::GetArchiveReader(const SevenZipLibraryManager &libraryManager, const CompressionFormat& format)
{
	const GUID* guid = GetCompressionGUID(format);

	CComPtr<IInArchive> archive;	
	libraryManager.CreateInterface(*guid, IID_IInArchive, reinterpret_cast<void**>(&archive));
	return archive;
}

CComPtr<IOutArchive> UsefulFunctions::GetArchiveWriter(const SevenZipLibraryManager &libraryManager, const CompressionFormat& format)
{
	const GUID* guid = GetCompressionGUID(format);

	CComPtr<IOutArchive> archive;
	libraryManager.CreateInterface(*guid, IID_IOutArchive, reinterpret_cast<void**>(&archive));
	return archive;
}

bool UsefulFunctions::GetNumberOfItems(const SevenZipLibraryManager  &libraryManager, const std::wstring & archivePath,
	CompressionFormat &format, size_t & numberofitems, const std::wstring& password)
{
	CComPtr<CInFileStream> fileStream = FileSys::OpenFileToRead(archivePath);

	if (fileStream == nullptr)
		return false;

	CComPtr<IInArchive> archive = UsefulFunctions::GetArchiveReader(libraryManager, format);
	if (!archive)
		return false;

	CComPtr<InStreamWrapper> inFile = new InStreamWrapper(fileStream);
	CComPtr<ArchiveOpenCallback> openCallback = new ArchiveOpenCallback(password);

	HRESULT hr = archive->Open(inFile, 0, openCallback);
	if (hr != S_OK)
		return false;

	UInt32 mynumofitems;
	hr = archive->GetNumberOfItems(&mynumofitems);
	if (hr != S_OK)
		return false;

	numberofitems = size_t(mynumofitems);

	archive->Close();
	return true;
}

bool UsefulFunctions::GetItemsNames(const SevenZipLibraryManager  &libraryManager, const std::wstring & archivePath, CompressionFormat &format, size_t & numberofitems, CObjectVector<std::wstring> & itemnames, CObjectVector<size_t> & origsizes, const std::wstring& password)
{
	CComPtr< CInFileStream > fileStream = FileSys::OpenFileToRead(archivePath);

	if (fileStream == NULL)
	{
		return false;
	}

	CComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(libraryManager, format);
	if (!archive)
	{
		return false;
	}

	CComPtr<InStreamWrapper> inFile = new InStreamWrapper(fileStream);
	CComPtr<ArchiveOpenCallback> openCallback = new ArchiveOpenCallback(password);

	HRESULT hr = archive->Open(inFile, 0, openCallback);
	if (hr != S_OK)
		return false;

	UInt32 mynumofitems;
	hr = archive->GetNumberOfItems(&mynumofitems);
	if (hr != S_OK)
		return false;

	numberofitems = size_t(mynumofitems);

	itemnames.ClearAndReserve(numberofitems);

	origsizes.ClearAndReserve(numberofitems);

	for (UInt32 i = 0; i < numberofitems; i++)
	{
		{
			// Get uncompressed size of file
			CPropVariant prop;
			hr = archive->GetProperty(i, kpidSize, &prop);
			if (hr != S_OK)
				return false;

			auto size = prop.uhVal.QuadPart;
			origsizes.Add(size_t(size));

			// Get name of file
			hr = archive->GetProperty(i, kpidPath, &prop);
			if (hr != S_OK)
				return false;

			//valid string? pass back the found value and call the callback function if set
			if (prop.vt == VT_BSTR) {
				std::wstring mypath(prop.bstrVal);
				itemnames.Add(mypath);
			}
		}
	}

	archive->Close();
	return true;
}

bool UsefulFunctions::DetectCompressionFormat(const SevenZipLibraryManager  &libraryManager, const std::wstring & archivePath,
	CompressionFormat & archiveCompressionFormat, const std::wstring& password)
{
	CComPtr< CInFileStream > fileStream = FileSys::OpenFileToRead(archivePath);

	if (fileStream == nullptr)
		return false;

	CObjectVector<CompressionFormat> myAvailableFormats;

	// Add more formats here if 7zip supports more formats in the future
	myAvailableFormats.Add(CompressionFormat::Zip);
	myAvailableFormats.Add(CompressionFormat::SevenZip);
	myAvailableFormats.Add(CompressionFormat::Rar);
	myAvailableFormats.Add(CompressionFormat::GZip);
	myAvailableFormats.Add(CompressionFormat::BZip2);
	myAvailableFormats.Add(CompressionFormat::Tar);
	myAvailableFormats.Add(CompressionFormat::Lzma);
	myAvailableFormats.Add(CompressionFormat::Lzma86);
	myAvailableFormats.Add(CompressionFormat::Cab);
	myAvailableFormats.Add(CompressionFormat::Iso);
	myAvailableFormats.Add(CompressionFormat::Arj);
	myAvailableFormats.Add(CompressionFormat::XZ);

	// Check each format for one that works
	for (size_t i = 0; i < myAvailableFormats.Size(); i++)
	{
		archiveCompressionFormat = myAvailableFormats[i];

		CComPtr<IInArchive> archive = UsefulFunctions::GetArchiveReader(libraryManager, archiveCompressionFormat);
		if (!archive) continue;

		CComPtr<InStreamWrapper> inFile = new InStreamWrapper(fileStream);
		CComPtr<ArchiveOpenCallback> openCallback = new ArchiveOpenCallback(password);

		const HRESULT hr = archive->Open(inFile, 0, openCallback);
		if (hr == S_OK)
		{
			// We know the format if we get here, so return
			archive->Close();
			return true;
		}

		archive->Close();
	}

	//
	//  There is a problem that GZip files will not be detected using the above method.
	//  This is a fix.
	//
	if (true)
	{
		size_t myNumOfItems = 0;
		archiveCompressionFormat = CompressionFormat::GZip;
		const bool result = GetNumberOfItems(libraryManager, archivePath, archiveCompressionFormat, myNumOfItems, password);
		if (result && myNumOfItems > 0)
		{
			// We know this file is a GZip file, so return
			return true;
		}
	}

	// If you get here, the format is unknown
	archiveCompressionFormat = CompressionFormat::Unknown;
	return false;
}

const std::wstring UsefulFunctions::EndingFromCompressionFormat(const CompressionFormat& format)
{
	switch (format)
	{
	case CompressionFormat::Zip:
		return _T(".zip");
		break;
	case CompressionFormat::SevenZip:
		return _T(".7z");
		break;
	case CompressionFormat::Rar:
		return _T(".rar");
		break;
	case CompressionFormat::GZip:
		return _T(".gz");
		break;
	case CompressionFormat::BZip2:
		return _T(".bz");
		break;
	case CompressionFormat::Tar:
		return _T(".tar");
		break;
	case CompressionFormat::Lzma:
		return _T(".lzma");
		break;
	case CompressionFormat::Lzma86:
		return _T(".lzma86");
		break;
	case CompressionFormat::Cab:
		return _T(".cab");
		break;
	case CompressionFormat::Iso:
		return _T(".iso");
		break;
	case CompressionFormat::Arj:
		return _T(".arj");
		break;
	case CompressionFormat::XZ:
		return _T(".xz");
		break;
	}
	return _T(".zip");
}

BSTR SevenZipInt::UsefulFunctions::StdStringAllocSysString(const std::wstring &string)
{
	return ::SysAllocString(string.c_str());
}

std::wstring SevenZipInt::UsefulFunctions::BstrToStdString(BSTR bstr)
{
	return bstr;
}