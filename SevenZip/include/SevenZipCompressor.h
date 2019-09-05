#pragma once

#include "FileInfo.h"
#include "SevenZipCpp.h"
#include "FileStreams.h"
#include "SevenZipArchive.h"
#include "ProgressCallback.h"
#include <codecvt>

#include <atlbase.h>



namespace SevenZip
{
	class SEVENZIPCPP_API SevenZipCompressor : public SevenZipInt::SevenZipArchive
	{
	public:
		// archive path - full path to creating archive without extension
		SevenZipCompressor(const SevenZipLibraryManager &library, const std::wstring &archivePath, bool useTempArchive = true);
		virtual ~SevenZipCompressor() = default;

		// Includes the directory as the root in the archive, e.g. specifying "C:\Temp\MyFolder"
		// makes "MyFolder" the single root item in archive with the files within it included.
		virtual bool AddDirectory(const std::wstring &directory, bool includeSubdirs = true);

		// Compress just this single file as the root item in the archive.
		virtual bool AddFile(const std::wstring &filePath);

		// Excludes the last directory as the root in the archive, its contents are at root instead. E.g.
		// specifying "C:\Temp\MyFolder" make the files in "MyFolder" the root items in the archive.
		virtual bool AddFiles(const std::wstring &directory, const std::wstring &searchFilter, bool includeSubdirs = true);
		virtual bool AddAllFiles(const std::wstring &directory, bool includeSubdirs = true);

		// Compress just this memory area as the root item in the archive.
		virtual bool AddMemory(const std::wstring &filePath, void* memPointer, size_t size);

		// Compress list of files
		virtual bool DoCompress(ProgressCallback* callback = nullptr);

		void ClearList() { _fileList.ClearAndFree(); }
		void UseAbsolutePaths(bool absolute) { _absolutePath = absolute; }
		bool CheckValidFormat() const;

	private:

		CComPtr<COutFileStream> OpenArchiveStream() const;

		bool AddFilesToList(const std::wstring &directory, const std::wstring &searchPattern, const std::wstring &pathPrefix, bool recursion);
		bool SetCompressionProperties(IUnknown* outArchive) const;

		bool _useTempArchive;
		bool _absolutePath;
		CObjectVector<FilePathInfo> _fileList; // list of files to compress
	};
}

