#include "stdafx.h"
#include "ArchiveExtractCallback.h"
#include "PropVariant.h"
#include "FileSys.h"
#include "OutStreamWrapper.h"
#include "UsefulFunctions.h"
#include <comdef.h>


using namespace SevenZipInt;
const std::wstring EmptyFileAlias = _T( "[Content]" );


ArchiveExtractCallback::ArchiveExtractCallback( const CComPtr< IInArchive > &archiveHandler, const std::wstring &directory, const std::wstring &archivePath, SevenZip::CompressionFormat compressionFormat, const std::wstring &password, ProgressCallback* callback)
	: _refCount(0)
	, _archiveHandler(archiveHandler)
	, _directory(directory)
	, _archivePath( archivePath )
	, _callback(callback)
	, _password(password)
	, _compressionFormat(compressionFormat)
{
}

STDMETHODIMP ArchiveExtractCallback::QueryInterface( REFIID iid, void** ppvObject )
{
	if ( iid == __uuidof( IUnknown ) )
	{
		*ppvObject = reinterpret_cast< IUnknown* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == IID_IArchiveExtractCallback )
	{
		*ppvObject = static_cast< IArchiveExtractCallback* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == IID_ICryptoGetTextPassword )
	{
		*ppvObject = static_cast< ICryptoGetTextPassword* >( this );
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ArchiveExtractCallback::AddRef()
{
	return static_cast< ULONG >( InterlockedIncrement( &_refCount ) );
}

STDMETHODIMP_(ULONG) ArchiveExtractCallback::Release()
{
	ULONG res = static_cast< ULONG >( InterlockedDecrement( &_refCount ) );
	if ( res == 0 )
	{
		delete this;
	}
	return res;
}

STDMETHODIMP ArchiveExtractCallback::SetTotal( UInt64 size )
{
	//	- SetTotal is never called for ZIP and 7z formats
	if (_callback != nullptr)
	{
		_callback->OnStartWithTotal(_absPath, size);
	}
	return CheckBreak();
}

STDMETHODIMP ArchiveExtractCallback::SetCompleted( const UInt64* completeValue )
{
	//Callback Event calls
	/*
	NB:
	- For ZIP format SetCompleted only called once per 1000 files in central directory and once per 100 in local ones.
	- For 7Z format SetCompleted is never called.
	*/
	if (_callback != nullptr)
	{
		//Don't call this directly, it will be called per file which is more consistent across archive types
		//TODO: incorporate better progress tracking
		//_callback->OnProgress(_absPath, *completeValue);
	}
	return CheckBreak();
}

STDMETHODIMP ArchiveExtractCallback::CheckBreak()
{
	if (_callback != nullptr)
	{
		// Abort if OnCheckBreak returns true;
		return _callback->OnCheckBreak() ? E_ABORT : S_OK;
	}
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::GetStream( UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode )
{
	try
	{
		// Retrieve all the various properties for the file at this index.
		GetPropertyFilePath( index );
		if ( askExtractMode != NArchive::NExtract::NAskMode::kExtract )
		{
			return S_OK;
		}

		GetPropertyAttributes( index );
		GetPropertyIsDir( index );
		GetPropertyModifiedTime( index );
		GetPropertySize( index );
	}
	catch ( _com_error &ex )
	{
		return ex.Error();
	}

	// Replace invalid characters
	for (unsigned iLetter = 0; iLetter < _relPath.length(); iLetter++)
	{
		TCHAR c = _relPath[iLetter];
		if (
			c == ':'
			|| c == '*'
			|| c == '?'
			|| c < 0x20 // printable character range starts at 20
			|| c == '<'
			|| c == '>'
			|| c == '|'
			|| c == '"'
			|| c == '/')
		{
			_relPath.replace(iLetter, 1, _T("_"));
		}
	}

	// TODO: _directory could be a relative path
	_absPath = FileSys::AppendPath( _directory, _relPath );

	if ( _isDir )
	{
		// Creating the directory here supports having empty directories.
		FileSys::CreateDirectoryTree( _absPath );
		*outStream = NULL;
		return S_OK;
	}

	std::wstring absDir = FileSys::GetPath( _absPath );
	FileSys::CreateDirectoryTree( absDir );

	CComPtr< COutFileStream > fileStream = FileSys::OpenFileToWrite( _absPath );
	if ( fileStream == nullptr )
	{
		_absPath.clear();
		return HRESULT_FROM_WIN32( GetLastError() );
	}

	CComPtr< OutStreamWrapper > wrapperStream = new OutStreamWrapper( fileStream );
	*outStream = wrapperStream.Detach();

	return CheckBreak();
}

STDMETHODIMP ArchiveExtractCallback::PrepareOperation( Int32 askExtractMode )
{
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetOperationResult( Int32 operationResult )
{
	if ( _absPath.empty() )
	{
		EmitDoneCallback();
		return CheckBreak();
	}

	if ( _hasModifiedTime )
	{
		HANDLE fileHandle = CreateFile( _absPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( fileHandle != INVALID_HANDLE_VALUE )
		{
			SetFileTime( fileHandle, NULL, NULL, &_modifiedTime );
			CloseHandle( fileHandle );
		}
	}

	if ( _hasAttrib )
	{
		SetFileAttributes( _absPath.c_str(), _attrib );
	}

	EmitFileDoneCallback(_absPath);
	return CheckBreak();
}

STDMETHODIMP ArchiveExtractCallback::CryptoGetTextPassword( BSTR* password )
{
	if (!_password.empty())
		*password = UsefulFunctions::StdStringAllocSysString(_password);

	return S_OK;
}

void ArchiveExtractCallback::GetPropertyFilePath( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = _archiveHandler->GetProperty( index, kpidPath, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	if ( prop.vt == VT_EMPTY )
	{
		if (_compressionFormat == CompressionFormat::GZip)
		{
			auto filename = _archivePath.substr(0, _archivePath.find_last_of('.'));
			filename = filename.substr(_archivePath.find_last_of('\\') + 1, filename.size());
			_relPath = filename;
		}
		else
			_relPath = EmptyFileAlias;
	}
	else if ( prop.vt != VT_BSTR )
	{
		_com_issue_error( E_FAIL );
	}
	else
	{
		_relPath = UsefulFunctions::BstrToStdString(prop.bstrVal);
	}
}

void ArchiveExtractCallback::GetPropertyAttributes( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = _archiveHandler->GetProperty( index, kpidAttrib, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	if ( prop.vt == VT_EMPTY )
	{
		_attrib = 0;
		_hasAttrib = false;
	}
	else if ( prop.vt != VT_UI4 )
	{
		_com_issue_error( E_FAIL );
	}
	else
	{
		_attrib = prop.ulVal;
		_hasAttrib = true;
	}
}

void ArchiveExtractCallback::GetPropertyIsDir( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = _archiveHandler->GetProperty( index, kpidIsDir, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	if ( prop.vt == VT_EMPTY )
	{
		_isDir = false;
	}
	else if ( prop.vt != VT_BOOL )
	{
		_com_issue_error( E_FAIL );
	}
	else
	{
		_isDir = prop.boolVal != VARIANT_FALSE;
	}
}

void ArchiveExtractCallback::GetPropertyModifiedTime( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = _archiveHandler->GetProperty( index, kpidMTime, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	if ( prop.vt == VT_EMPTY )
	{
		_hasModifiedTime = false;
	}
	else if ( prop.vt != VT_FILETIME )
	{
		_com_issue_error( E_FAIL );
	}
	else
	{
		_modifiedTime = prop.filetime;
		_hasModifiedTime = true;
	}
}

void ArchiveExtractCallback::GetPropertySize( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = _archiveHandler->GetProperty( index, kpidSize, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	switch ( prop.vt )
	{
	case VT_EMPTY:
		_hasNewFileSize = false;
		return;
	case VT_UI1:
		_newFileSize = prop.bVal;
		break;
	case VT_UI2:
		_newFileSize = prop.uiVal;
		break;
	case VT_UI4:
		_newFileSize = prop.ulVal;
		break;
	case VT_UI8:
		_newFileSize = (UInt64)prop.uhVal.QuadPart;
		break;
	default:
		_com_issue_error( E_FAIL );
	}

	_hasNewFileSize = true;
}

void ArchiveExtractCallback::EmitDoneCallback()
{
	if (_callback != nullptr)
	{
		_callback->OnDone(_directory);
	}
}

void ArchiveExtractCallback::EmitFileDoneCallback(const std::wstring &path)
{
	if (_callback != nullptr)
	{
		_callback->OnProgress(_archivePath, _newFileSize);
		_callback->OnFileDone(_archivePath, path, _newFileSize);
	}
}
