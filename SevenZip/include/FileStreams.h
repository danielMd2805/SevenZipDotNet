// FileStreams.h
#ifndef __FILE_STREAMS_H
#define __FILE_STREAMS_H

#ifdef _WIN32
#define USE_WIN_FILE
#endif

#include "MyString.h"

#include "FileIO.h"

#include "Alloc.h"

#include <atlbase.h>

//#include "../../Common/MyCom.h"


typedef UINT_PTR My_UINT_PTR;

#include <initguid.h>

#define MY_QUERYINTERFACE_BEGIN STDMETHOD(QueryInterface) \
(REFGUID iid, void **outObject) throw() { *outObject = NULL;

#define MY_QUERYINTERFACE_ENTRY(i) else if (iid == IID_ ## i) \
    { *outObject = (void *)(i *)this; }

#define MY_QUERYINTERFACE_ENTRY_UNKNOWN(i) if (iid == IID_IUnknown) \
    { *outObject = (void *)(IUnknown *)(i *)this; }

#define MY_QUERYINTERFACE_BEGIN2(i) MY_QUERYINTERFACE_BEGIN \
    MY_QUERYINTERFACE_ENTRY_UNKNOWN(i) \
    MY_QUERYINTERFACE_ENTRY(i)

#define MY_QUERYINTERFACE_END else return E_NOINTERFACE; ++__m_RefCount; /* AddRef(); */ return S_OK; }

#define MY_ADDREF_RELEASE \
STDMETHOD_(ULONG, AddRef)() throw() { return ++__m_RefCount; } \
STDMETHOD_(ULONG, Release)() { if (--__m_RefCount != 0)  \
  return __m_RefCount; delete this; return 0; }

#define MY_UNKNOWN_IMP MY_QUERYINTERFACE_BEGIN \
  MY_QUERYINTERFACE_ENTRY_UNKNOWN(IUnknown) \
  MY_QUERYINTERFACE_END \
  MY_ADDREF_RELEASE

#define MY_UNKNOWN_IMP1(i) MY_UNKNOWN_IMP_SPEC( \
  MY_QUERYINTERFACE_ENTRY_UNKNOWN(i) \
  MY_QUERYINTERFACE_ENTRY(i) \
  )

#define MY_UNKNOWN_IMP_SPEC(i) \
  MY_QUERYINTERFACE_BEGIN \
  i \
  MY_QUERYINTERFACE_END \
  MY_ADDREF_RELEASE


#include "7z/CPP/7zip/IStream.h"

class CMyUnknownImp
{
public:
	ULONG __m_RefCount;
	CMyUnknownImp() : __m_RefCount(0) {}

	// virtual
	~CMyUnknownImp() {}
};

struct IInFileStream_Callback
{
  virtual HRESULT InFileStream_On_Error(My_UINT_PTR val, DWORD error) = 0;
  virtual void InFileStream_On_Destroy(My_UINT_PTR val) = 0;
};

class CInFileStream:
  public IInStream,
  public IStreamGetSize,
  public IStreamGetProps,
  public IStreamGetProps2,
  public CMyUnknownImp
{
public:
  NWindows::NFile::NIO::CInFile File;
  
  UInt64 VirtPos;
  UInt64 PhyPos;
  UInt64 BufStartPos;
  Byte *Buf;
  UInt32 BufSize;

  bool SupportHardLinks;

  IInFileStream_Callback *Callback;
  My_UINT_PTR CallbackRef;

  virtual ~CInFileStream();

  CInFileStream();
  
  bool Open(CFSTR fileName)
  {
    return File.Open(fileName);
  }
  
  bool OpenShared(CFSTR fileName, bool shareForWrite)
  {
    return File.OpenShared(fileName, shareForWrite);
  }

  MY_QUERYINTERFACE_BEGIN2(IInStream)
  MY_QUERYINTERFACE_ENTRY(IStreamGetSize)
  MY_QUERYINTERFACE_ENTRY(IStreamGetProps)
  MY_QUERYINTERFACE_ENTRY(IStreamGetProps2)
  MY_QUERYINTERFACE_END
  MY_ADDREF_RELEASE

  STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);

  STDMETHOD(GetSize)(UInt64 *size);
  STDMETHOD(GetProps)(UInt64 *size, FILETIME *cTime, FILETIME *aTime, FILETIME *mTime, UInt32 *attrib);
  STDMETHOD(GetProps2)(CStreamFileProps *props);
};

class CStdInFileStream:
  public ISequentialInStream,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP

  virtual ~CStdInFileStream() {}
  STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
};

class COutFileStream:
  public IOutStream,
  public CMyUnknownImp
{
public:
  NWindows::NFile::NIO::COutFile File;
  
  virtual ~COutFileStream() {}
  bool Create(CFSTR fileName, bool createAlways)
  {
    ProcessedSize = 0;
    return File.Create(fileName, createAlways);
  }
  bool Open(CFSTR fileName, DWORD creationDisposition)
  {
    ProcessedSize = 0;
    return File.Open(fileName, creationDisposition);
  }

  HRESULT Close();
  
  UInt64 ProcessedSize;

  #ifdef USE_WIN_FILE
  bool SetTime(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime)
  {
    return File.SetTime(cTime, aTime, mTime);
  }
  bool SetMTime(const FILETIME *mTime) {  return File.SetMTime(mTime); }
  #endif


  MY_UNKNOWN_IMP1(IOutStream)

  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
  STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
  STDMETHOD(SetSize)(UInt64 newSize);

  HRESULT GetSize(UInt64 *size);
};

class CStdOutFileStream:
  public ISequentialOutStream,
  public CMyUnknownImp
{
  UInt64 _size;
public:
  MY_UNKNOWN_IMP

  UInt64 GetSize() const { return _size; }
  CStdOutFileStream(): _size(0) {}
  virtual ~CStdOutFileStream() {}
  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
};

#endif
