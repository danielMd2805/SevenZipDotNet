# SevenZipDotNet - wrapping original 7-zip API of Igor Pavlov 
**Use original 7-zip api in .Net and C++ with identical performance of 7-zip GUI**

This repo contains the library implementation of 7z.dll for .NET Standard, .NET Core, .NET Framework and CPP.
The library is CLR managed to be accessed from CPP and C#.

## How to use in code

To use this library you have to specifiy the path to the 7z.dll of the original 7-Zip GUI of Igor Pavlov.

#### 7-Zip License
https://7-zip.org/license.txt

### .NET

#### Compression

```c#
SevenZipCompressor compressor = new SevenZipCompressor(@"PATH_TO_SEVENZIPDLL_\7z.dll",
                                                    @"D:\Temp\Test\test.xml");

compressor.SetCompressionFormat(CompressionFormat.GZip);
compressor.SetCompressionLevel(CompressionLevel.Fast);

compressor.AddFile(@"D:\Temp\Test\test.xml");
compressor.DoCompress();
watch.Stop();
```

#### Decompression

```c#
SevenZipExtractor extractor = new SevenZipExtractor(@"PATH_TO_SEVENZIPDLL_\7z.dll",
                                                        @"D:\Temp\Test\test.xml.gz");

extractor.SetCompressionFormat(CompressionFormat.GZip);

extractor.ExtractArchive(@"D:\Temp\Test\out");
```

### CPP

Using the library from CPP first you have to specify the include directory to SevenZip source code.

```cpp

#include <SevenZip/include/SevenZipLibraryManager.h>
#include <SevenZip/include/SevenZipCompressor.h>
#include <SevenZip/include/SevenZipExtractor.h>

```

After specifing the include path you have to call the SevenZipLibraryManager.

```cpp

SevenZipLibraryManager *libManager = new SevenZipLibraryManager(LR"(PATH_TO_SEVENZIPDLL_\7z.dll)");

```

#### Compression

```cpp

SevenZipCompressor *compressor = new SevenZipCompressor(*libManager, LR"(D:\Temp\Test\test.xml)");
compressor->SetCompressionFormat(CompressionFormat::GZip);
compressor->SetCompressionLevel(CompressionLevel::Fast);
compressor->AddFile(LR"(D:\Temp\Test\test.xml)");
compressor->DoCompress();

```

#### Decompression

```cpp

SevenZipExtractor *extractor = new SevenZipExtractor(*libManager, LR"(D:\Temp\Test\test.xml.gz)");
extractor->SetCompressionFormat(CompressionFormat::GZip);
extractor->ExtractArchive(LR"(D:\Temp\Test\out)")
```
