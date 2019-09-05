#pragma once


#include "7z/CPP/7zip/Archive/IArchive.h"

#include "CompressionFormat.h"
#include "SevenZipLibraryManager.h"
#include "MyVector.h"

#include <string>

namespace SevenZipInt
{
	using namespace SevenZip;
	class UsefulFunctions
	{
	public:
		static const GUID* GetCompressionGUID(const CompressionFormat &format);

		static CComPtr< IInArchive > GetArchiveReader(const SevenZipLibraryManager &libraryManager, const CompressionFormat &format);
		static CComPtr< IOutArchive > GetArchiveWriter(const SevenZipLibraryManager &libraryManager, const CompressionFormat &format);

		static bool DetectCompressionFormat(const SevenZipLibraryManager &libraryManager, const std::wstring &archivePath, CompressionFormat &archiveCompressionFormat, const std::wstring &password);

		static bool GetNumberOfItems(const SevenZipLibraryManager &libraryManager, const std::wstring  &archivePath, CompressionFormat  &format, size_t  &numberofitems, const std::wstring &password);

		static bool GetItemsNames(const SevenZipLibraryManager &libraryManager, const std::wstring  &archivePath, CompressionFormat  &format, size_t  &numberofitems, CObjectVector<std::wstring>  &itemnames, CObjectVector<size_t>  &origsizes, const std::wstring &password);

		static const std::wstring EndingFromCompressionFormat(const CompressionFormat &format);

		static BSTR StdStringAllocSysString(const std::wstring &string);
		static std::wstring BstrToStdString(BSTR string);
	};
}
