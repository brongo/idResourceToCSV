#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "Config.h"
#include "ResourceFile.h"

namespace ResourceToCSV 
{
    typedef unsigned char byte;
    typedef unsigned int uint32;
    typedef unsigned __int64 uint64;

    class ResourceParser
    {
        public:

            Config config;
            ResourceEntryList masterList;

        public:

            // sorting and duplicate removal - helper functions
            void sortResourceListAlpha();
            void sortResourceListPriority();
            void removeDuplicatesAlpha();
            void removeDuplicatesPriority();
            void removeDuplicatesLeftover();

            // sorting and duplicate removal - main function
            void sortAndRemoveDuplicates();

            // write functions
            void writeResourceListToCSV(char* logfile);

            // builds initial resource list from file
            void buildResourceList();

            // main parser function
            void parseResourceFiles(std::vector<std::string> inputFileList, char* logfile);
    };
}