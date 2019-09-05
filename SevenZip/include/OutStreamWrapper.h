#pragma once

#include "stdafx.h"

#include <7z/CPP/7zip/IStream.h>
#include "FileStreams.h"


namespace SevenZipInt
{
	class OutStreamWrapper : public IOutStream
	{
	public:

		OutStreamWrapper(CComPtr<COutFileStream> baseStream);
		virtual ~OutStreamWrapper();

		STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// ISequentialOutStream
		STDMETHOD(Write)(const void* data, UInt32 size, UInt32* processedSize);

		// IOutStream
		STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition);
		STDMETHOD(SetSize)(UInt64 newSize);

		void CloseStream();

	private:

		long						_refCount;
		CComPtr<COutFileStream>			_baseStream;
	};
}
