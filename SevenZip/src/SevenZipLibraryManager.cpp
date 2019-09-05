#include "stdafx.h"

#include "SevenZipLibraryManager.h"
#include "MyVector.h"

#include <iostream>
#include <vector>

using namespace SevenZip;

SevenZipLibraryManager::SevenZipLibraryManager(const std::wstring &libraryPath)
	: _libHandle(nullptr)
	, _createObjectFunc(nullptr)
{
	if (!LoadLibraryInt(libraryPath))
		return;
}


SevenZipLibraryManager::~SevenZipLibraryManager()
{
	if (_libHandle != nullptr)
		FreeLibrary(_libHandle);

	_createObjectFunc = nullptr;
}

bool SevenZipLibraryManager::LoadLibraryInt(const std::wstring &libraryPath)
{
	std::vector<wchar_t> vec(libraryPath.begin(), libraryPath.end());
	vec.push_back(L'\0');
	const LPWSTR dllPath = &vec[0];

	const HINSTANCE hGetProcIdDll = LoadLibrary(dllPath);

	dllPath[libraryPath.size()] = NULL;

	if (!hGetProcIdDll)
	{
		const int errorCode = GetLastError();

		WriteError(L"Could not load the dynamic library");
		WriteError(L"ErrorCode: " + std::to_wstring(errorCode));
		WriteInfo(libraryPath);

		return false;
	}

	const FARPROC functionPtr = GetProcAddress(hGetProcIdDll, "GetHandlerProperty");
	if (!functionPtr)
	{
		WriteError(L"Could not locate the function 'GetHandlerProperty()'");
		FreeLibrary(hGetProcIdDll);
		return false;
	}

	_libHandle = hGetProcIdDll;

	_createObjectFunc = reinterpret_cast<CreateObjectFunc>(GetProcAddress(_libHandle, "CreateObject"));
	if (_createObjectFunc == nullptr)
	{
		FreeLibrary(_libHandle);
		return false;
	}

	return true;
}


bool SevenZipLibraryManager::CreateInterface(const GUID &classID, const GUID &interfaceID, void **outObject) const
{
	if (_libHandle == nullptr)
	{
		WriteError(L"Invalid library handle. Library loaded?");
		return false;
	}

	if (_createObjectFunc == nullptr)
	{
		WriteError(L"Invalid library handle. Library loaded?");
		return false;
	}

	HRESULT hr = _createObjectFunc(&classID, &interfaceID, outObject);

	if (FAILED(hr))
	{
		WriteError(L"Could not load interface.");
		return false;
	}

	return true;
}

void SevenZipLibraryManager::WriteError(const std::wstring &message)
{
	std::wcout << "[Error] " << "SevenZipLibraryManager: " << message << std::endl;
}

void SevenZipLibraryManager::WriteInfo(const std::wstring &message)
{
	std::wcout << "[Info] " << "SevenZipLibraryManager: " << message << std::endl;
}


