#include "StdAfx.h"
#include "MemExtractCallback.h"
#include "PropVariant.h"
#include "OutMemStream.h"
#include "UsefulFunctions.h"

#include <comdef.h>


namespace SevenZipInt
{
	const std::wstring EmptyFileAlias = _T("[Content]");


	MemExtractCallback::MemExtractCallback(const CComPtr< IInArchive > &archiveHandler,
										   SevenZipMemoryQueue &bufferQueue,
										   int bufferSize,	
										   const std::wstring &archivePath,
										   const std::wstring &password,
										   ProgressCallback* callback)
		: m_refCount(0)
		, m_archiveHandler(archiveHandler)
		, m_bufferQueue(bufferQueue)
		, m_archivePath(archivePath)
		, m_callback(callback)
		, m_password(password)
		, m_bufferSize(bufferSize)
	{
	}

	STDMETHODIMP MemExtractCallback::QueryInterface(REFIID iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = reinterpret_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}

		if (iid == IID_IArchiveExtractCallback)
		{
			*ppvObject = static_cast<IArchiveExtractCallback*>(this);
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

	STDMETHODIMP_(ULONG) MemExtractCallback::AddRef()
	{
		return static_cast<ULONG>(InterlockedIncrement(&m_refCount));
	}

	STDMETHODIMP_(ULONG) MemExtractCallback::Release()
	{
		ULONG res = static_cast<ULONG>(InterlockedDecrement(&m_refCount));
		if (res == 0)
		{
			delete this;
		}
		return res;
	}

	STDMETHODIMP MemExtractCallback::SetTotal(UInt64 size)
	{
		//	- SetTotal is never called for ZIP and 7z formats
		if (m_callback != nullptr)
		{
			m_callback->OnStartWithTotal(m_archivePath, size);
		}
		return CheckBreak();
	}

	STDMETHODIMP MemExtractCallback::SetCompleted(const UInt64* completeValue)
	{
		//Callback Event calls
		/*
		NB:
		- For ZIP format SetCompleted only called once per 1000 files in central directory and once per 100 in local ones.
		- For 7Z format SetCompleted is never called.
		*/
		if (m_callback != nullptr)
		{
			//Don't call this directly, it will be called per file which is more consistent across archive types
			//TODO: incorporate better progress tracking
			//m_callback->OnProgress(m_absPath, *completeValue);
		}
		return CheckBreak();
	}

	STDMETHODIMP MemExtractCallback::CheckBreak()
	{
		if (m_callback != nullptr)
		{
			// Abort if OnCheckBreak returns true;
			return m_callback->OnCheckBreak() ? E_ABORT : S_OK;
		}
		return S_OK;
	}

	STDMETHODIMP MemExtractCallback::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode)
	{
		try
		{
			// Retrieve all the various properties for the file at this index.
			GetPropertyFilePath(index);
			if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
			{
				return S_OK;
			}

			GetPropertyIsDir(index);
			GetPropertySize(index);
		}
		catch (_com_error &ex)
		{
			return ex.Error();
		}

		if (!m_isDir)
		{
			CComPtr<ISequentialOutStream> outStreamLoc(new COutMemStream(m_bufferQueue, m_bufferSize));
			m_outMemStream = outStreamLoc;
			*outStream = outStreamLoc.Detach();
		}


		return CheckBreak();
	}

	STDMETHODIMP MemExtractCallback::PrepareOperation(Int32 askExtractMode)
	{
		return S_OK;
	}

	STDMETHODIMP MemExtractCallback::SetOperationResult(Int32 operationResult)
	{
		int errors = 0;
		switch (operationResult)
		{
		case NArchive::NExtract::NOperationResult::kOK:
			break;

		default:
			{
				errors++;
				switch (operationResult)
				{
				case NArchive::NExtract::NOperationResult::kUnsupportedMethod:
					break;

				case NArchive::NExtract::NOperationResult::kCRCError:
					break;

				case NArchive::NExtract::NOperationResult::kDataError:
					break;

				default:;
				}
			}
		}

		m_outMemStream.Release();

		if (errors)
		{
			return E_FAIL;
		}

		EmitFileDoneCallback(_T(""));
		return CheckBreak();
	}

	STDMETHODIMP MemExtractCallback::CryptoGetTextPassword(BSTR* password)
	{
		if (!m_password.empty())
			*password = UsefulFunctions::StdStringAllocSysString(m_password);

		return S_OK;
	}

	void MemExtractCallback::GetPropertyFilePath(UInt32 index)
	{
		CPropVariant prop;
		HRESULT hr = m_archiveHandler->GetProperty(index, kpidPath, &prop);
		if (hr != S_OK)
		{
			_com_issue_error(hr);
		}

		if (prop.vt == VT_EMPTY)
		{
			m_filePath = EmptyFileAlias;
		}
		else if (prop.vt != VT_BSTR)
		{
			_com_issue_error(E_FAIL);
		}
		else
		{
			m_filePath = UsefulFunctions::BstrToStdString(prop.bstrVal);
		}
	}

	void MemExtractCallback::GetPropertyIsDir(UInt32 index)
	{
		CPropVariant prop;
		HRESULT hr = m_archiveHandler->GetProperty(index, kpidIsDir, &prop);
		if (hr != S_OK)
		{
			_com_issue_error(hr);
		}

		if (prop.vt == VT_EMPTY)
		{
			m_isDir = false;
		}
		else if (prop.vt != VT_BOOL)
		{
			_com_issue_error(E_FAIL);
		}
		else
		{
			m_isDir = prop.boolVal != VARIANT_FALSE;
		}
	}

	void MemExtractCallback::GetPropertySize(UInt32 index)
	{
		CPropVariant prop;
		HRESULT hr = m_archiveHandler->GetProperty(index, kpidSize, &prop);
		if (hr != S_OK)
		{
			_com_issue_error(hr);
		}

		switch (prop.vt)
		{
		case VT_EMPTY:
			m_hasNewFileSize = false;
			return;
		case VT_UI1:
			m_newFileSize = prop.bVal;
			break;
		case VT_UI2:
			m_newFileSize = prop.uiVal;
			break;
		case VT_UI4:
			m_newFileSize = prop.ulVal;
			break;
		case VT_UI8:
			m_newFileSize = static_cast<UInt64>(prop.uhVal.QuadPart);
			break;
		default:
			_com_issue_error(E_FAIL);
		}

		m_hasNewFileSize = true;
	}

	void MemExtractCallback::EmitDoneCallback() const
	{
		if (m_callback != nullptr)
		{
			m_callback->OnDone(_T(""));
		}
	}

	void MemExtractCallback::EmitFileDoneCallback(const std::wstring &path) const
	{
		if (m_callback != nullptr)
		{
			m_callback->OnProgress(m_archivePath, m_newFileSize);
			m_callback->OnFileDone(m_archivePath, path, m_newFileSize);
		}
	}

}
