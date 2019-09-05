#include "stdafx.h"

#include "ArchiveOpenCallback.h"
#include "UsefulFunctions.h"

using namespace SevenZipInt;

ArchiveOpenCallback::ArchiveOpenCallback(const std::wstring &password)
	: _refCount(0)
{
}

STDMETHODIMP ArchiveOpenCallback::QueryInterface(REFIID iid, void** ppvObject)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvObject = reinterpret_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_IArchiveOpenCallback)
	{
		*ppvObject = static_cast<IArchiveOpenCallback*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_ICryptoGetTextPassword)
	{
		*ppvObject = static_cast<ICryptoGetTextPassword*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ArchiveOpenCallback::AddRef()
{
	return static_cast<ULONG>(InterlockedIncrement(&_refCount));
}

STDMETHODIMP_(ULONG) ArchiveOpenCallback::Release()
{
	ULONG res = static_cast<ULONG>(InterlockedDecrement(&_refCount));
	if (res == 0)
		delete this;
	return res;
}

STDMETHODIMP ArchiveOpenCallback::SetTotal(const UInt64* files, const UInt64* bytes)
{
	return S_OK;
}

STDMETHODIMP ArchiveOpenCallback::SetCompleted(const UInt64* files, const UInt64* bytes)
{
	return S_OK;
}

STDMETHODIMP ArchiveOpenCallback::CryptoGetTextPassword(BSTR* password)
{
	if (!_password.empty())
		*password = UsefulFunctions::StdStringAllocSysString(_password);

	return S_OK;
}

