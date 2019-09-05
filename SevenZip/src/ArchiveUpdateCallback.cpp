#include "stdafx.h"
#include "ArchiveUpdateCallback.h"
#include "FileSys.h"
#include "InStreamWrapper.h"
#include "PropVariant.h"
#include "UsefulFunctions.h"


#include "GUIDs.h"

using namespace SevenZipInt;

ArchiveUpdateCallback::ArchiveUpdateCallback(const CObjectVector<FilePathInfo> &filePaths, const std::wstring &outputFilePath, const std::wstring &password, ProgressCallback* callback)
	: _refCount(0)
	, _filePaths(filePaths)
	, _callback(callback)
	, _outputPath(outputFilePath)
	, _password(password)
{
}

STDMETHODIMP ArchiveUpdateCallback::QueryInterface(REFIID iid, void** ppvObject)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvObject = reinterpret_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_IArchiveUpdateCallback)
	{
		*ppvObject = static_cast<IArchiveUpdateCallback*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_ICryptoGetTextPassword2)
	{
		*ppvObject = static_cast<ICryptoGetTextPassword2*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_ICompressProgressInfo)
	{
		*ppvObject = static_cast<ICompressProgressInfo*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ArchiveUpdateCallback::AddRef()
{
	return static_cast<ULONG>(InterlockedIncrement(&_refCount));
}

STDMETHODIMP_(ULONG) ArchiveUpdateCallback::Release()
{
	ULONG res = static_cast<ULONG>(InterlockedDecrement(&_refCount));
	if (res == 0)
	{
		delete this;
	}
	return res;
}

STDMETHODIMP ArchiveUpdateCallback::SetTotal(UInt64 size)
{
	if (_callback != nullptr)
	{
		_callback->OnStartWithTotal(_outputPath, size);
	}
	return CheckBreak();
}

STDMETHODIMP ArchiveUpdateCallback::SetCompleted(const UInt64* completeValue)
{
	if (_callback != nullptr)
	{
		_callback->OnProgress(_outputPath, *completeValue);
	}
	return CheckBreak();
}

STDMETHODIMP ArchiveUpdateCallback::CheckBreak()
{
	if (_callback != nullptr)
	{
		// Abort if OnCheckBreak returns true;
		return _callback->OnCheckBreak() ? E_ABORT : S_OK;
	}
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::GetUpdateItemInfo(UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive)
{
	// Setting info for Create mode (vs. Append mode).
	// TODO: support append mode
	if (newData != NULL)
	{
		*newData = 1;
	}

	if (newProperties != NULL)
	{
		*newProperties = 1;
	}

	if (indexInArchive != NULL)
	{
		*indexInArchive = static_cast<UInt32>(-1); // TODO: UInt32.Max
	}

	return CheckBreak();
}

STDMETHODIMP ArchiveUpdateCallback::GetProperty(UInt32 index, PROPID propID, PROPVARIANT* value)
{
	CPropVariant prop;

	if (propID == kpidIsAnti)
	{
		prop = false;
		prop.Detach(value);
		return S_OK;
	}

	if (index>= _filePaths.Size())
	{
		return E_INVALIDARG;
	}

	const FilePathInfo &fileInfo = _filePaths[index];
	switch (propID)
	{
	case kpidPath:		prop = FileSys::ExtractRelativePath(fileInfo.rootPath, fileInfo.FilePath).c_str(); break;
	case kpidIsDir:		prop = fileInfo.IsDirectory; break;
	case kpidSize:		prop = fileInfo.Size; break;
	case kpidAttrib:	prop = fileInfo.Attributes; break;
	case kpidCTime:		prop = fileInfo.CreationTime; break;
	case kpidATime:		prop = fileInfo.LastAccessTime; break;
	case kpidMTime:		prop = fileInfo.LastWriteTime; break;
	default: ;
	}

	prop.Detach(value);
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::GetStream(UInt32 index, ISequentialInStream** inStream)
{
	if (index>= _filePaths.Size())
	{
		return E_INVALIDARG;
	}

	const FilePathInfo &fileInfo = _filePaths[index];
	if (fileInfo.IsDirectory)
	{
		return S_OK;
	}

	HRESULT hr = S_OK;
	CComPtr<CInFileStream> outStream;
	if (fileInfo.memFile)
	{
		/*const HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, static_cast<SIZE_T>(fileInfo.Size));
		if (!hGlobal)
		{
			return E_OUTOFMEMORY;
		}

		void* lpData = GlobalLock(hGlobal);
		memcpy(lpData, fileInfo.memPointer, static_cast<SIZE_T>(fileInfo.Size));
		GlobalUnlock(hGlobal);
		hr = CreateStreamOnHGlobal(hGlobal, TRUE, &outStream);*/
	}
	else
	{
		outStream = FileSys::OpenFileToRead(fileInfo.FilePath);
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

	if (!outStream)
	{
		return hr;
	}

	CComPtr<InStreamWrapper> wrapperStream = new InStreamWrapper(outStream);
	*inStream = wrapperStream.Detach();

	return CheckBreak();
}

STDMETHODIMP ArchiveUpdateCallback::SetOperationResult(Int32 operationResult)
{
	return CheckBreak();
}

STDMETHODIMP ArchiveUpdateCallback::CryptoGetTextPassword2(Int32* passwordIsDefined, BSTR* password)
{
	if (!_password.empty())
		*password = UsefulFunctions::StdStringAllocSysString(_password);

	*passwordIsDefined = _password.empty() ? 0 : 1;

	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::SetRatioInfo(const UInt64* inSize, const UInt64* outSize)
{
	return CheckBreak();
}

