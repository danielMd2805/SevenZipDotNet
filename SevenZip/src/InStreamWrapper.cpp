#include "stdafx.h"

#include "InStreamWrapper.h"
#include "FileStreams.h"

namespace SevenZipInt
{
	
InStreamWrapper::InStreamWrapper(CComPtr<CInFileStream> baseStream)
	: _refCount(0)
	, _baseStream(baseStream)
{
}


InStreamWrapper::~InStreamWrapper()
= default;

HRESULT STDMETHODCALLTYPE InStreamWrapper::QueryInterface(REFIID iid, void** ppvObject)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvObject = reinterpret_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_ISequentialInStream)
	{
		*ppvObject = static_cast<ISequentialInStream*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_IInStream)
	{
		*ppvObject = static_cast<IInStream*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_IStreamGetSize)
	{
		*ppvObject = static_cast<IStreamGetSize*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE InStreamWrapper::AddRef()
{
	return static_cast<ULONG>(InterlockedIncrement(&_refCount));
}

ULONG STDMETHODCALLTYPE InStreamWrapper::Release()
{
	ULONG res = static_cast<ULONG>(InterlockedDecrement(&_refCount));
	if (res == 0)
	{
		delete this;
	}
	return res;
}

STDMETHODIMP InStreamWrapper::Read(void* data, UInt32 size, UInt32* processedSize)
{
	/*ULONG read = 0;	
	const HRESULT hr = _baseStream->Read(data, size, &read);
	if (processedSize != nullptr)
		*processedSize = read;*/
	const HRESULT hr = _baseStream->Read(data, size, processedSize);

	return SUCCEEDED(hr) ? S_OK : hr;
}

STDMETHODIMP InStreamWrapper::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
{
	/*LARGE_INTEGER move;
	ULARGE_INTEGER newPos;

	move.QuadPart = offset;
	const HRESULT hr = _baseStream->Seek(move, seekOrigin, &newPos);
	if (newPosition != nullptr)
		*newPosition = newPos.QuadPart;*/

	const HRESULT hr = _baseStream->Seek(offset, seekOrigin, newPosition);
	return hr;
}

STDMETHODIMP InStreamWrapper::GetSize(UInt64* size)
{
	/*STATSTG statInfo;
	HRESULT hr = _baseStream->Stat(&statInfo, STATFLAG_NONAME);
	if (SUCCEEDED(hr))
		*size = statInfo.cbSize.QuadPart;*/

	HRESULT hr = _baseStream->GetSize(size);
	return hr;
}
}
