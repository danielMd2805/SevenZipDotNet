#pragma once

#include "stdafx.h"
#include <deque>
#include <string>


namespace SevenZip {
	struct FilePathInfo;
}

namespace SevenZipInt
{
	struct FilePathInfo;

	class PathScanner
	{
	public:

		class Callback
		{
		public:

			virtual ~Callback() {}

			virtual void BeginScan() {}
			virtual void EndScan() {}
			virtual bool ShouldDescend(const SevenZip::FilePathInfo &directory) = 0;
			virtual void EnterDirectory(const std::wstring &path) {}
			virtual void LeaveDirectory(const std::wstring &path) {}
			virtual void ExamineFile(const SevenZip::FilePathInfo &file, bool &exit) = 0;
		};

	public:

		static void Scan(const std::wstring &root, const std::wstring &pathPrefix, Callback &cb);
		static void Scan(const std::wstring &root, const std::wstring &pathPrefix, const std::wstring &searchPattern, Callback &cb);

	private:

		static bool ExamineFiles(const std::wstring &directory, const std::wstring &searchPattern, const std::wstring &pathPrefix, Callback &cb);
		static void ExamineDirectories(const std::wstring &directory, std::deque< std::wstring > &subDirs, const std::wstring &pathPrefix, Callback &cb);

		static bool IsAllFilesPattern(const std::wstring &searchPattern);
		static bool IsSpecialFileName(const std::wstring &fileName);
		static bool IsDirectory(const WIN32_FIND_DATA &fdata);
		static SevenZip::FilePathInfo ConvertFindInfo(const std::wstring &directory, const std::wstring &pathPrefix, const WIN32_FIND_DATA &fdata);
	};
}

