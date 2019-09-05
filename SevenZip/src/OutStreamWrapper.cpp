#include "stdafx.h"
#include "OutStreamWrapper.h"
#include <utility>

using namespace SevenZipInt;

OutStreamWrapper::OutStreamWrapper(CComPtr<COutFileStream> baseStream)
	: _refCount(0)
	, _baseStream(baseStream)
{
	
}

OutStreamWrapper::~OutStreamWrapper()
= default;

HRESULT STDMETHODCALLTYPE OutStreamWrapper::QueryInterface(REFIID iid, void** ppvObject)
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

	if (iid == IID_IOutStream)
	{
		*ppvObject = static_cast<IOutStream*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE OutStreamWrapper::AddRef()
{
	return static_cast<ULONG>(InterlockedIncrement(&_refCount));
}

ULONG STDMETHODCALLTYPE OutStreamWrapper::Release()
{
	ULONG res = static_cast<ULONG>(InterlockedDecrement(&_refCount));
	if (res == 0)
	{
		CloseStream();
		delete this;
	}
	return res;
}

STDMETHODIMP OutStreamWrapper::Write(const void* data, UInt32 size, UInt32* processedSize)
{
	//ULONG written = 0;
	//HRESULT hr = _baseStream->Write(data, size, &written);
	HRESULT hr = _baseStream->Write(data, size, processedSize);
	/*if (processedSize != NULL)
	{
		*processedSize = written;
	}*/
	return hr;
}

STDMETHODIMP OutStreamWrapper::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
{
	/*LARGE_INTEGER move;
	ULARGE_INTEGER newPos;

	move.QuadPart = offset;*/
	//HRESULT hr = _baseStream->Seek(move, seekOrigin, &newPos);
	HRESULT hr = _baseStream->Seek(offset, seekOrigin, newPosition);
	/*if (newPosition != NULL)
	{
		*newPosition = newPos.QuadPart;
	}*/
	return hr;
}

STDMETHODIMP OutStreamWrapper::SetSize(UInt64 newSize)
{
	/*ULARGE_INTEGER size;
	size.QuadPart = newSize;
	return _baseStream->SetSize(size);*/
	return _baseStream->SetSize(newSize);
}

void OutStreamWrapper::CloseStream()
{
	if (_baseStream)
		_baseStream->Close();
}

