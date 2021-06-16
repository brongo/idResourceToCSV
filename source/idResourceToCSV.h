#pragma once
#define WIN32_LEAN_AND_MEAN
#define SAFE_SPACE 64
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include "ResourceParser.h"

namespace ResourceToCSV
{
	namespace fs = std::filesystem;
	class InputValidator
	{
		public:
			std::vector<std::string> fileList;

		public:

			// input validation helpers
			void closeProgramAfterSleep(int sleepTime);
			bool getFileList(fs::path& inputPath);
			bool inputIsDirectory(fs::path& inputPath) const;
			bool inputIsEternalDirectory(fs::path& inputPath) const;
			bool inputHasResourceExtension(fs::path& inputPath) const;
			bool outputFileAlreadyExists(char* logfile) const;

			// main validation function
			bool inputIsValid(char* inputPath, char* logfile);
	};
}
