#pragma once

#include "stdafx.h"

#include "SevenZipCpp.h"

#include <string>



namespace SevenZip
{

	class SEVENZIPCPP_API SevenZipLibraryManager
	{
	public:
		SevenZipLibraryManager(const std::wstring &libraryPath);
		~SevenZipLibraryManager();

		bool LibraryLoaded() const { return _libHandle != nullptr; }

		bool CreateInterface(const GUID &classID, const GUID &interfaceID, void **outObject) const;

	private:
		typedef UINT32(WINAPI * CreateObjectFunc)(const GUID* clsID, const GUID* interfaceID, void** outObject);

		HINSTANCE			_libHandle;
		CreateObjectFunc	_createObjectFunc;

	private:
		bool LoadLibraryInt(const std::wstring &libraryPath);

		static void WriteError(const std::wstring &message);
		static void WriteInfo(const std::wstring &message);
	};
}




