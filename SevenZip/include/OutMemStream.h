#pragma once

#include <7z/CPP/7zip/IStream.h>

#include <vector>
#include <atlbase.h>
#include "BlockingQueue.h"
#include "SevenZipMemoryQueue.h"

namespace SevenZipInt
{
	class COutMemStream : public ISequentialOutStream
	{
	public:

		explicit COutMemStream(SevenZip::SevenZipMemoryQueue &bufferQueue, int bufferSize = INT_MAX / 4);
		virtual ~COutMemStream();

		STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// ISequentialOutStream
		STDMETHOD(Write)(const void* data, UInt32 size, UInt32* processedSize);

	private:

		long _refCount;
		int _bufferCount;
		int _bufferSize;

		SevenZip::SevenZipMemoryQueue &_bufferQueue;
		std::vector<unsigned char> *_buffer;
	};
}
