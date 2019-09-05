#pragma once


#include <7z/CPP/7zip/Archive/IArchive.h>
#include <7z/CPP/7zip/IPassword.h>
//#include <7z/CPP/7zip/UI/Common/UpdateCallback.h>

#include "ProgressCallback.h"

#include <vector>
#include <string>
#include "BlockingQueue.h"
#include "SevenZipMemoryQueue.h"

namespace SevenZipInt
{
	class MemExtractCallback : public IArchiveExtractCallback, public ICryptoGetTextPassword
	{
	private:

		long m_refCount;
		int m_bufferSize;

		CComPtr< IInArchive > m_archiveHandler;
		CComPtr< ISequentialOutStream > m_outMemStream;
		SevenZip::SevenZipMemoryQueue &m_bufferQueue;

		std::wstring m_archivePath;
		std::wstring m_password;

		bool m_isDir;
		bool m_hasNewFileSize;
		UInt64 m_newFileSize;
		std::wstring m_filePath;

		SevenZip::ProgressCallback* m_callback;

	public:

		MemExtractCallback(const CComPtr< IInArchive > &archiveHandler, SevenZip::SevenZipMemoryQueue &bufferQueue, int bufferSize, const std::wstring &archivePath, const std::wstring &password, SevenZip::ProgressCallback* callback);
		virtual ~MemExtractCallback() = default;

		STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// IProgress
		STDMETHOD(SetTotal)( UInt64 size );
		STDMETHOD(SetCompleted)( const UInt64* completeValue );

		// Early exit, this is not part of any interface
		STDMETHOD(CheckBreak)();

		// IMemExtractCallback
		STDMETHOD(GetStream)( UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode );
		STDMETHOD(PrepareOperation)( Int32 askExtractMode );
		STDMETHOD(SetOperationResult)( Int32 resultEOperationResult );

		// ICryptoGetTextPassword
		STDMETHOD(CryptoGetTextPassword)( BSTR* password );

	private:

		void GetPropertyFilePath( UInt32 index );
		void GetPropertyIsDir( UInt32 index );
		void GetPropertySize( UInt32 index );

		void EmitDoneCallback() const;
		void EmitFileDoneCallback(const std::wstring &path) const;
	};
}
