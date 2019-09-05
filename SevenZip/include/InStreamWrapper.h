#pragma once

#include "stdafx.h"

#include "7z/CPP/7zip/IStream.h"

#include <atlbase.h>
#include "FileStreams.h"

namespace SevenZipInt
{
	class InStreamWrapper : public IInStream, public IStreamGetSize
	{
	public:
		InStreamWrapper(CComPtr<CInFileStream> baseStream);
		virtual ~InStreamWrapper();

		STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// ISequentialInStream
		STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize);

		// IInStream
		STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition);

		// IStreamGetSize
		STDMETHOD(GetSize)(UInt64* size);

	private:
		long _refCount;
		CComPtr<CInFileStream> _baseStream;
	};
}
