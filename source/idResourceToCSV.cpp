#include "idResourceToCSV.h"
using namespace ResourceToCSV;

namespace ResourceToCSV
{
    // input validation helpers
    void InputValidator::closeProgramAfterSleep(int sleepTime)
    {
        printf("Program will exit now...\n\n\n");
        auto sleepTime_ms = std::chrono::milliseconds(sleepTime);
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(sleepTime_ms);
    }
    bool InputValidator::getFileList(fs::path& inputPath)
    {
        fs::path basedir = inputPath / "base";
        int searchIterations = 0;

        for (const auto& p : fs::recursive_directory_iterator(basedir))
        {
            if (p.path().extension() == ".resources")
                fileList.push_back(p.path().string());

            // abort early if too many files/folders found (any type).
            if (searchIterations > 4000) 
            {
                printf("ERROR: Too many files in /base/ directory (over 4,000). \n");
                printf("Program or search may be corrupted somehow. Aborting for safety reasons. \n");
                return 0;
            }
            searchIterations++;
        }
        return 1;
    }
    bool InputValidator::inputIsDirectory(fs::path& inputPath) const
    {
        if (fs::is_directory(inputPath))
            return 1;

        printf("ERROR: The path you provided is invalid. \n");
        printf("Path must be either a .resource file or the Doom Eternal main directory. \n");
        return 0;
    }
    bool InputValidator::inputIsEternalDirectory(fs::path& inputPath) const
    {
        fs::path doomexe = inputPath / "DOOMEternalx64vk.exe";
        fs::path basepath = inputPath / "base";

        if (fs::exists(doomexe) && fs::exists(basepath))
            return 1;

        printf("ERROR: The path you provided is invalid. \n");
        printf("Path must be either a .resource file or the Doom Eternal main directory. \n");
        return 0;
    }
    bool InputValidator::inputHasResourceExtension(fs::path& inputPath) const
    {
        if (inputPath.extension() == ".resources")
            return 1;

        if (inputPath.extension() == ".backup")
            return 1;

        return 0;
    }
    bool InputValidator::outputFileAlreadyExists(char* logfile) const
    {
        fs::path outputFile = logfile;
        if (!fs::exists(outputFile))
            return 0;

        printf("ERROR: Output file already exists. Please choose a different filename. \n");
        return 1;
    }

    // main validator function
    bool InputValidator::inputIsValid(char* input, char* logfile)
    {
        fs::path inputPath = input;

        if (outputFileAlreadyExists(logfile))
            return 0;

        if (inputHasResourceExtension(inputPath))
        {
            fileList.push_back(std::string(input));
            return 1;
        }

        if (!inputIsDirectory(inputPath))
            return 0;

        if (!inputIsEternalDirectory(inputPath))
            return 0;

        if (getFileList(inputPath))
            return 1;

        return 0;
    }

}

int main(int argc, char* argv[]) 
{
    printf("idResourceToCSV v0.1 by SamPT\n"); 
    if (argc < 2) 
    {
        printf("USAGE: idResourceToCSV.exe path output_file \n");
        printf("Path should be a single resource file, or Doom Eternal main directory. \n");
        return 1;
    }

    char defaultFile[20] = "idResourceToCSV.csv";
    char* logfile = defaultFile;

    if (argc == 3)
        logfile = argv[2];

    InputValidator inputValidator;
    if (inputValidator.inputIsValid(argv[1], logfile))
    {
        ResourceParser parser;
        parser.parseResourceFiles(inputValidator.fileList, logfile);
        return 0;
    }

    inputValidator.closeProgramAfterSleep(2000);
    return 1;
}