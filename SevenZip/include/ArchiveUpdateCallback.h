#pragma once

#include "stdafx.h"

#include <7z/CPP/7zip/Archive/IArchive.h>
#include <7z/CPP/7zip/IPassword.h>
#include <7z/CPP/7zip/ICoder.h>

#include "FileInfo.h"
#include "MyVector.h"
#include "ProgressCallback.h"


namespace SevenZipInt
{
	class ArchiveUpdateCallback  : public IArchiveUpdateCallback,  public ICryptoGetTextPassword2, public ICompressProgressInfo
	{
	private:

		long _refCount;
		std::wstring _outputPath;
		std::wstring _password;
		const CObjectVector<SevenZip::FilePathInfo> &_filePaths;
		SevenZip::ProgressCallback* _callback;

	public:

		ArchiveUpdateCallback(const CObjectVector<SevenZip::FilePathInfo> &filePaths, const std::wstring &outputFilePath, const std::wstring &password, SevenZip::ProgressCallback* callback);
		virtual ~ArchiveUpdateCallback() = default;

		STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// IProgress
		STDMETHOD(SetTotal)(UInt64 size);
		STDMETHOD(SetCompleted)(const UInt64* completeValue);

		// Early exit, this is not part of any interface
		STDMETHOD(CheckBreak)();

		// IArchiveUpdateCallback
		STDMETHOD(GetUpdateItemInfo)(UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive);
		STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT* value);
		STDMETHOD(GetStream)(UInt32 index, ISequentialInStream** inStream);
		STDMETHOD(SetOperationResult)(Int32 operationResult);

		// ICryptoGetTextPassword2
		STDMETHOD(CryptoGetTextPassword2)(Int32* passwordIsDefined, BSTR* password);

		// ICompressProgressInfo
		STDMETHOD(SetRatioInfo)(const UInt64* inSize, const UInt64* outSize);
	};
}

