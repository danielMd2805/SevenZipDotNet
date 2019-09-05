#pragma once


#include <7z/CPP/7zip/Archive/IArchive.h>
#include <7z/CPP/7zip/IPassword.h>

#include "ProgressCallback.h"
#include "CompressionFormat.h"


namespace SevenZipInt
{
	class ArchiveExtractCallback : public IArchiveExtractCallback, public ICryptoGetTextPassword
	{
	private:

		long _refCount;
		CComPtr< IInArchive > _archiveHandler;
		std::wstring _directory;

		std::wstring _password;

		std::wstring _relPath;
		std::wstring _absPath;
		bool _isDir;

		std::wstring _archivePath;

		bool _hasAttrib;
		UInt32 _attrib;

		bool _hasModifiedTime;
		FILETIME _modifiedTime;

		bool _hasNewFileSize;
		UInt64 _newFileSize;

		SevenZip::ProgressCallback* _callback;
		SevenZip::CompressionFormat _compressionFormat;

	public:

		ArchiveExtractCallback(const CComPtr< IInArchive > &archiveHandler, const std::wstring &directory, const std::wstring &archivePath, SevenZip::CompressionFormat compressionFormat,  const std::wstring &password, SevenZip::ProgressCallback* callback);
		virtual ~ArchiveExtractCallback() = default;

		STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// IProgress
		STDMETHOD(SetTotal)( UInt64 size );
		STDMETHOD(SetCompleted)( const UInt64* completeValue );

		// Early exit, this is not part of any interface
		STDMETHOD(CheckBreak)();

		// IArchiveExtractCallback
		STDMETHOD(GetStream)( UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode );
		STDMETHOD(PrepareOperation)( Int32 askExtractMode );
		STDMETHOD(SetOperationResult)( Int32 resultEOperationResult );

		// ICryptoGetTextPassword
		STDMETHOD(CryptoGetTextPassword)( BSTR* password );

	private:

		void GetPropertyFilePath( UInt32 index );
		void GetPropertyAttributes( UInt32 index );
		void GetPropertyIsDir( UInt32 index );
		void GetPropertyModifiedTime( UInt32 index );
		void GetPropertySize( UInt32 index );

		void EmitDoneCallback();
		void EmitFileDoneCallback(const std::wstring &path);
	};
}
