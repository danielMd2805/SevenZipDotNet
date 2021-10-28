#pragma once
#include "SevenZipCompressor.h"
#include "SevenZipExtractor.h"
#include "SevenZipMemoryQueue.h"


namespace SevenZip
{
	namespace CLI
	{
		ref class SevenZipMemoryQueue;

		public enum class CompressionLevel
		{
			None,
			Fast,
			Low,
			Normal,
			High,
			Ultra
		};

		public enum class CompressionFormat
		{
			Unknown,
			SevenZip,
			Zip,
			GZip,
			BZip2,
			Rar,
			Tar,
			Iso,
			Cab,
			Lzma,
			Lzma86,
			Arj,
			XZ,
		};

		public ref class SevenZipArchive
		{
		public:
			SevenZipArchive(SevenZipInt::SevenZipArchive *impl);

			bool ReadInArchiveMetadata();

			void SetCompressionFormat(CompressionFormat format);

			CompressionFormat GetCompressionFormat();

			void SetCompressionLevel(CompressionLevel level);
			CompressionLevel GetCompressionLevel();

			bool DetectCompressionFormat();

			System::Int64 GetNumberOfItems();
			System::Collections::Generic::List<System::String^>^ GetItemsNames();
			System::Collections::Generic::List<System::Int64>^  GetOrigSizes();
			void SetPassword(System::String^ password);
			void DisableEndingWithCompressionFormat();

		protected:
			std::wstring StringToWString(System::String^ string);
			System::String^ WStringToString(std::wstring string);

		private:
			SevenZipInt::SevenZipArchive *_implArch;
		};

		public ref class SevenZipCompressor : public SevenZipArchive
		{
		public:
			SevenZipCompressor(System::String^ libraryPath, System::String^ archivePath);

			bool AddDirectory(System::String^ directory, bool includeSubdirs);

			bool AddFile(System::String^ filePath);

			bool AddFiles(System::String^ directory, System::String^ searchFilter, bool includeSubdirs);

			bool AddAllFiles(System::String^ directory, bool includeSubdirs);

			bool AddMemory(System::String^ filePath, System::IntPtr data, System::Int64 size);

			bool DoCompress();

			void ClearList();

			void UseAbsolutePaths(bool absolute);

			bool CheckValidFormat();

		private:
			SevenZip::SevenZipLibraryManager *_libraryManager;
			SevenZip::SevenZipCompressor *_impl;
		};

		using namespace System::Runtime::InteropServices;

		public ref class SevenZipExtractor : public SevenZipArchive
		{
		public:
			SevenZipExtractor(System::String^ libraryPath, System::String^ archivePath);

			bool ExtractArchive(System::String^ directory);

			bool ExtractFilesFromArchive(System::UInt32 fileIndices, System::UInt32 numberFiles, System::String^ directory);

			bool ExtractFileToMemory(System::UInt32 index, SevenZipMemoryQueue^ memory_queue, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<int> bufferSize);

		private:
			SevenZip::SevenZipLibraryManager *_libraryManager;
			SevenZip::SevenZipExtractor *_impl;
		};
		
		public ref class SevenZipMemoryQueue 
		{
		public:
			SevenZipMemoryQueue();
			SevenZipMemoryQueue(int maxQueueSize);

			void Add(array<System::Byte>^ item);

			bool TryTake([Out] System::IO::UnmanagedMemoryStream^% result, [Out]intptr_t^% handle);

			void CompleteAdding();

			bool Completed();

			void FreeHandle(intptr_t^ handle);
			SevenZip::SevenZipMemoryQueue* Impl();

		private:
			void Init();
			void Init(int maxQueueSize);

		private:
			SevenZip::SevenZipMemoryQueue *_impl;
			
		};
	}
}

