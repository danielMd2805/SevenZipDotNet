#include "stdafx.h"

#include "OutMemStream.h"
#include <iostream>

namespace SevenZipInt
{
	COutMemStream::COutMemStream(SevenZip::SevenZipMemoryQueue &bufferQueue, int bufferSize)
		: _bufferQueue(bufferQueue)
		, _refCount(0)
		, _buffer(new std::vector<unsigned char>())
		, _bufferCount(0)
		, _bufferSize(bufferSize)
	{
		
	}

	COutMemStream::~COutMemStream()
	{
	}

	STDMETHODIMP COutMemStream::QueryInterface(REFIID iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}

		if (iid == IID_ISequentialOutStream)
		{
			*ppvObject = static_cast<ISequentialOutStream*>(this);
			AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) COutMemStream::AddRef()
	{
		return static_cast<ULONG>(InterlockedIncrement(&_refCount));
	}

	STDMETHODIMP_(ULONG) COutMemStream::Release()
	{
		ULONG res = static_cast<ULONG>(InterlockedDecrement(&_refCount));
		if (res == 0)
		{
			if (_bufferCount > 0)
			{
				_bufferQueue.Add(*_buffer);
			}
			delete _buffer;
			delete this;
		}
		return res;
	}

	STDMETHODIMP COutMemStream::Write(const void* data, UInt32 size, UInt32* processedSize)
	{
		if (processedSize != nullptr)
		{
			*processedSize = 0;
		}
		if (data == nullptr || size == 0)
		{
			return E_FAIL;
		}
		if (_bufferCount >= _bufferSize - size)
		{	
			_bufferQueue.Add(*_buffer);
			_buffer->clear();
			_bufferCount = 0;
		}

		const unsigned char* byte_data = static_cast<const unsigned char*>(data);

		_buffer->insert(_buffer->end(), byte_data, byte_data + size);

		_bufferCount += size;

		*processedSize = size;

		return S_OK;
	}
}
