#pragma once


#include "SevenZipLibraryManager.h"
#include "SevenZipArchive.h"
#include "SevenZipCpp.h"
#include "ProgressCallback.h"
#include "BlockingQueue.h"
#include "SevenZipMemoryQueue.h"

#include <vector>
#include "FileStreams.h"



namespace SevenZip
{
	class SEVENZIPCPP_API SevenZipExtractor : public SevenZipInt::SevenZipArchive
	{
	public:

		SevenZipExtractor(const SevenZipLibraryManager &library, const std::wstring &archivePath);
		virtual ~SevenZipExtractor();

		virtual bool ExtractArchive(const std::wstring &directory, ProgressCallback* callback = nullptr);
		virtual bool ExtractFilesFromArchive(const unsigned int* fileIndices,
			const unsigned int numberFiles,
			const std::wstring &directory,
			ProgressCallback* callback = nullptr);
		virtual bool ExtractFileToMemory(const unsigned int index, SevenZipMemoryQueue &bufferQueue, int bufferSize = INT_MAX / 4, ProgressCallback* callback = nullptr);

	private:
		bool ExtractFilesFromArchive(const CComPtr<CInFileStream> &archiveStream,
			const unsigned int* fileIndices,
			const unsigned int numberFiles,
			const std::wstring &directory,
			ProgressCallback* callback) const;
	};
}

