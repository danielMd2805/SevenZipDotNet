
//#include <string>
#include <stdlib.h>

#include <msclr/marshal_cppstd.h>

#include "SevenZipCompressorCLI.h"
#include "BlockingQueue.h"
#include <iostream>

using namespace SevenZip::CLI;

SevenZipArchive::SevenZipArchive(SevenZipInt::SevenZipArchive* impl)
	: _implArch(impl)
{
}

bool SevenZipArchive::ReadInArchiveMetadata()
{
	return _implArch->ReadInArchiveMetadata();
}

void SevenZipArchive::SetCompressionFormat(CompressionFormat format)
{
	_implArch->SetCompressionFormat(SevenZip::CompressionFormat(format));
}

CompressionFormat SevenZipArchive::GetCompressionFormat()
{
	return CompressionFormat(_implArch->GetCompressionFormat());
}

void SevenZipArchive::SetCompressionLevel(CompressionLevel level)
{
	_implArch->SetCompressionLevel(SevenZip::CompressionLevel(level));
}

CompressionLevel SevenZipArchive::GetCompressionLevel()
{
	return CompressionLevel(_implArch->GetCompressionLevel());
}

bool SevenZipArchive::DetectCompressionFormat()
{
	return _implArch->DetectCompressionFormat();
}

System::Int64 SevenZipArchive::GetNumberOfItems()
{
	return _implArch->GetNumberOfItems();
}

void SevenZipArchive::SetPassword(System::String^ password)
{
	_implArch->SetPassword(StringToWString(password));
}

System::Collections::Generic::List<System::String^>^ SevenZipArchive::GetItemsNames()
{
	auto names = _implArch->GetItemsNames();

	System::Collections::Generic::List<System::String^>^ result = gcnew System::Collections::Generic::List<System::String^>(names.Size());

	for (unsigned int i = 0; i < names.Size(); i++)
	{
		result->Add(WStringToString(names[i]));
	}

	return result;
}

System::Collections::Generic::List<System::Int64>^ SevenZipArchive::GetOrigSizes()
{
	auto sizes = _implArch->GetOrigSizes();

	System::Collections::Generic::List<System::Int64>^ result = gcnew System::Collections::Generic::List<System::Int64>(sizes.Size());

	for (unsigned int i = 0; i < sizes.Size(); i++)
	{
		result->Add(sizes[i]);
	}

	return result;

}

std::wstring SevenZipArchive::StringToWString(System::String^ string)
{
	return msclr::interop::marshal_as<std::wstring>(string);
}

System::String^ SevenZipArchive::WStringToString(std::wstring string)
{
	return msclr::interop::marshal_as<System::String^>(string);
}


SevenZipCompressor::SevenZipCompressor(::System::String^ libraryPath, ::System::String^ archivePath)
	: _libraryManager(new SevenZip::SevenZipLibraryManager(StringToWString(libraryPath))),
	  _impl(new SevenZip::SevenZipCompressor(*_libraryManager, StringToWString(archivePath))),
	  SevenZipArchive(_impl)
{
}

bool SevenZipCompressor::AddDirectory(System::String^ directory, bool includeSubdirs)
{
	return _impl->AddDirectory(StringToWString(directory), includeSubdirs);
}

bool SevenZipCompressor::AddFile(System::String^ filePath)
{
	return _impl->AddFile(StringToWString(filePath));
}

bool SevenZipCompressor::AddFiles(System::String^ directory, System::String^ searchFilter, bool includeSubdirs)
{
	return _impl->AddFiles(StringToWString(directory), StringToWString(searchFilter), includeSubdirs);
}

bool SevenZipCompressor::AddAllFiles(System::String^ directory, bool includeSubdirs)
{
	return _impl->AddAllFiles(StringToWString(directory), includeSubdirs);
}

bool SevenZipCompressor::AddMemory(System::String^ filePath, System::IntPtr data, System::Int64 size)
{
	return _impl->AddMemory(StringToWString(filePath), data.ToPointer(), size);
}

bool SevenZipCompressor::DoCompress()
{
	return _impl->DoCompress(nullptr);
}

void SevenZipCompressor::ClearList()
{
	_impl->ClearList();
}

void SevenZipCompressor::UseAbsolutePaths(bool absolute)
{
	_impl->UseAbsolutePaths(absolute);
}

bool SevenZipCompressor::CheckValidFormat()
{
	return _impl->CheckValidFormat();
}


SevenZipExtractor::SevenZipExtractor(System::String^ libraryPath, System::String^ archivePath)
	: _libraryManager(new SevenZip::SevenZipLibraryManager(StringToWString(libraryPath)))
	, _impl(new SevenZip::SevenZipExtractor(*_libraryManager, StringToWString(archivePath)))
	, SevenZipArchive(_impl)
{
}

bool SevenZipExtractor::ExtractArchive(System::String^ directory)
{
	return _impl->ExtractArchive(StringToWString(directory), nullptr);
}

bool SevenZipExtractor::ExtractFilesFromArchive(System::UInt32 fileIndices, System::UInt32 numberFiles, System::String^ directory)
{
	return _impl->ExtractFilesFromArchive(&fileIndices, numberFiles, StringToWString(directory));
}

bool SevenZipExtractor::ExtractFileToMemory(System::UInt32 index, SevenZipMemoryQueue^ memory_queue, System::Nullable<int> bufferSize)
{
	if (bufferSize.HasValue)
		return _impl->ExtractFileToMemory(index, *memory_queue->Impl(), bufferSize.Value);
	else
		return _impl->ExtractFileToMemory(index, *memory_queue->Impl());
}


SevenZipMemoryQueue::SevenZipMemoryQueue()
{
	Init();
}
SevenZipMemoryQueue::SevenZipMemoryQueue(int maxQueueSize)
{
	Init(maxQueueSize);
}

void SevenZipMemoryQueue::Add(array<System::Byte>^ item)
{
	//Currently not supported

	/*pin_ptr<unsigned char> pin(&item[0]);
	unsigned char *p2 = pin;
	_impl->Add(p2);*/
}

bool SevenZipMemoryQueue::TryTake(System::IO::UnmanagedMemoryStream^% result, intptr_t^% handle)
{
	std::vector<unsigned char> *data = new std::vector<unsigned char>();
	if (_impl->TryTake(data))
	{
		result = gcnew System::IO::UnmanagedMemoryStream(data->data(), data->size());
		handle = reinterpret_cast<intptr_t>(data);
		
		return true;
	}
	result = nullptr;
	return false;
}

void SevenZipMemoryQueue::FreeHandle(intptr_t^ handle)
{
	auto pointer = reinterpret_cast<std::vector<unsigned char>*>(static_cast<intptr_t>(handle));
	delete pointer;
}


void SevenZipMemoryQueue::CompleteAdding()
{
	_impl->CompleteAdding();
}

bool SevenZipMemoryQueue::Completed()
{
	return _impl->Completed();
}

SevenZip::SevenZipMemoryQueue* SevenZipMemoryQueue::Impl()
{
	return _impl;
}

void SevenZipMemoryQueue::Init()
{
	Init(5);
}
void SevenZipMemoryQueue::Init(int maxQueueSize)
{
	_impl = new SevenZip::SevenZipMemoryQueue(maxQueueSize);
}





