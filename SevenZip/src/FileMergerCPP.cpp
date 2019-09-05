// FileMergerCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <string>

#include "SevenZipLibraryManager.h"

using namespace SevenZip;

static void WriteMessage(const std::string &message)
{
	std::cout << "[Info]" << message << std::endl;
}

static void WriteError(const std::string &message)
{
	std::cout << "[Error] " << message << std::endl;
}

int main()
{
	std::cout << "Bewotec FileMerger - CPP Edition\n";

	WriteMessage("Loading 7-zip library...");

	SevenZipLibraryManager *libManager = new SevenZipLibraryManager(R"(C:\Users\Daniels\Downloads\7z1806-src\CPP\7zip\UI\GUI\x64\Debug\7z.dll)");

	if (libManager->LibraryLoaded())
		WriteMessage("Loading 7-ip library was successfully.");
	else
		WriteError("Loading 7-zip library failed.");
	
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
