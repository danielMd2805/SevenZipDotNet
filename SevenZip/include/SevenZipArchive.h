#pragma once

#include "stdafx.h"

#include "SevenZipLibraryManager.h"
#include "CompressionFormat.h"
#include "CompressionLevel.h"

#include "MyVector.h"

#include <string>


namespace SevenZipInt
{
	class SEVENZIPCPP_API SevenZipArchive
	{
	public:
		SevenZipArchive(const SevenZip::SevenZipLibraryManager &libraryManager, const  std::wstring &archivePath);
		virtual ~SevenZipArchive();

		virtual bool ReadInArchiveMetadata();

		virtual void SetCompressionFormat(const SevenZip::CompressionFormat &format);
		virtual SevenZip::CompressionFormat GetCompressionFormat();

		virtual void SetCompressionLevel(const SevenZip::CompressionLevel &level);
		virtual SevenZip::CompressionLevel GetCompressionLevel();

		virtual bool DetectCompressionFormat();

		virtual size_t GetNumberOfItems();
		virtual CObjectVector<std::wstring> GetItemsNames();
		virtual CObjectVector<size_t>  GetOrigSizes();
		virtual void SetPassword(const std::wstring& password);

		virtual void DisableEndingWithCompressionFormat();

	protected:
		bool _ReadMetadata;
		bool _OverrideCompressionFormat;
		bool _endingWithCompressionFormat;
		
		const SevenZip::SevenZipLibraryManager &_libraryManager;

		std::wstring _archivePath;

		SevenZip::CompressionFormat _compressionFormat;
		SevenZip::CompressionLevel _compressionLevel;

		size_t _numberOfItems;

		CObjectVector<std::wstring> _itemNames;
		CObjectVector<size_t> _orgSizes;

		std::wstring _password;

	private:
		bool GetNumberOfItemsInt();
		bool GetItemsNamesInt();
		bool DetectCompressionFormat(SevenZip::CompressionFormat &format);
		bool DetectCompressionFormatInt();

	};

}
