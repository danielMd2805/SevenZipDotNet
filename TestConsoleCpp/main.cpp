// FileMergerNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <string>
#include <chrono>

#include <SevenZip/include/SevenZipLibraryManager.h>
#include <SevenZip/include/SevenZipCompressor.h>
#include <SevenZip/include/SevenZipExtractor.h>

using namespace SevenZip;

static void WriteMessage(const std::wstring &message)
{
	std::wcout << "[Info] " << message << std::endl;
}

static void WriteError(const std::wstring &message)
{
	std::wcout << "[Error] " << message << std::endl;
}

int main()
{
	std::wcout << "SevenZipDotNet - CPP Test Edition" << std::endl;
	std::wcout << "================================" << std::endl;
	std::wcout << std::endl;

	WriteMessage(L"Loading 7-zip library...");

	SevenZipLibraryManager *libManager = new SevenZipLibraryManager(LR"(PATH_TO_SEVENZIPDLL_\7z.dll)");

	if (libManager->LibraryLoaded())
		WriteMessage(L"Loading 7-zip library was successfully.");
	else
		WriteError(L"Loading 7-zip library failed.");

	SevenZipCompressor *compressor = new SevenZipCompressor(*libManager, LR"(D:\Temp\Test\test.xml)");
	compressor->SetCompressionFormat(CompressionFormat::GZip);
	compressor->SetCompressionLevel(CompressionLevel::Fast);
	compressor->AddFile(LR"(D:\Temp\Test\test.xml)");

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	if (compressor->DoCompress())
		WriteMessage(L"Compressing successfully.");
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	WriteMessage(L"Compressing tooks: " + std::to_wstring(time_span.count()));

	SevenZipExtractor *extractor = new SevenZipExtractor(*libManager, LR"(D:\Temp\Test\test.xml.gz)");
	extractor->SetCompressionFormat(CompressionFormat::GZip);

	t1 = std::chrono::high_resolution_clock::now();
	if (extractor->ExtractArchive(LR"(D:\Temp\Test\out)"))
		WriteMessage(L"Extraction successfully.");
	t2 = std::chrono::high_resolution_clock::now();

	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	WriteMessage(L"Extraction tooks: " + std::to_wstring(time_span.count()));

	std::wcout << "Press <Enter> for exit..." << std::endl;
	getchar();

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
