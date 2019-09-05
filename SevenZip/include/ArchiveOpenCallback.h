#pragma once

#include "stdafx.h"

#include "7z\CPP\7zip\Archive\IArchive.h"
#include "7z\CPP\7zip\IPassword.h"

#include <string>
#include <atlbase.h>

namespace SevenZipInt
{

	class ArchiveOpenCallback : public IArchiveOpenCallback, public ICryptoGetTextPassword
	{
	public:
		ArchiveOpenCallback(const std::wstring &password);
		virtual ~ArchiveOpenCallback() = default;

		STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// IArchiveOpenCallback
		STDMETHOD(SetTotal)(const UInt64* files, const UInt64* bytes);
		STDMETHOD(SetCompleted)(const UInt64* files, const UInt64* bytes);

		// ICryptoGetTextPassword
		STDMETHOD(CryptoGetTextPassword)(BSTR* password);


	private:
		long _refCount;
		std::wstring _password;
	};
}
