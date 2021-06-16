#include "ResourceParser.h"

namespace ResourceToCSV
{
    // sorting and duplicate removal functions
    void ResourceParser::sortResourceListAlpha()
    {
        sort(masterList.resourceList.begin(), masterList.resourceList.end(), [](const ResourceEntry& a, const ResourceEntry& b) {
            if (a.name != b.name)
                return (a.name < b.name);
            return (a.type < b.type);
            });
        return;
    }
    void ResourceParser::sortResourceListPriority()
    {
        sort(masterList.resourceList.begin(), masterList.resourceList.end(), [](const ResourceEntry& a, const ResourceEntry& b) {
            if (a.name != b.name)
                return (a.name < b.name);
            return (a.priority < b.priority);
            });
        return;
    }
    void ResourceParser::removeDuplicatesAlpha()
    {
        auto it = std::unique(masterList.resourceList.begin(), masterList.resourceList.end(), [](const ResourceEntry& a, const ResourceEntry& b) {
            if ((a.name == b.name) && (a.type == b.type) && (a.version == b.version) && (a.hash == b.hash))
                return 1;
            return 0;
            });

        size_t listSize = it - masterList.resourceList.begin();
        masterList.resourceList.resize(listSize);
        masterList.numResourceEntries = masterList.resourceList.size();
        return;
    }
    void ResourceParser::removeDuplicatesPriority()
    {
        // Removes duplicates with lowest load priority
        auto it = std::unique(masterList.resourceList.begin(), masterList.resourceList.end(), [](const ResourceEntry& a, const ResourceEntry& b) {
            if ((a.name == b.name) && (a.type == b.type) && (a.priority < b.priority))
                return 1;
            return 0;
            });

        size_t listSize = it - masterList.resourceList.begin();
        masterList.resourceList.resize(listSize);
        masterList.numResourceEntries = masterList.resourceList.size();
        return;
    }
    void ResourceParser::removeDuplicatesLeftover()
    {
        // remove any leftover duplicates by name
        auto it = std::unique(masterList.resourceList.begin(), masterList.resourceList.end(), [](const ResourceEntry& a, const ResourceEntry& b) {
            if (a.name == b.name)
                return 1;
            return 0;
            });

        size_t listSize = it - masterList.resourceList.begin();
        masterList.resourceList.resize(listSize);
        masterList.numResourceEntries = masterList.resourceList.size();
        return;
    }
    void ResourceParser::sortAndRemoveDuplicates()
    {
        // should always be used in this order
        printf("Sorting and removing duplicates, please wait. \n");
        sortResourceListAlpha();
        removeDuplicatesAlpha();
        sortResourceListPriority();
        removeDuplicatesPriority();
        removeDuplicatesLeftover();
    }

    // write functions
    void ResourceParser::writeResourceListToCSV(char* logfile)
    {
        std::ofstream outputFile(logfile, std::ios::app);

        // write csv headers
        std::string headers = "";
        headers = "name,type,hash,version,filenum,havokFlag1,havokFlag2,havokFlag3\n";
        outputFile.write(headers.c_str(), headers.length());

        // write csv data
        for (int i = 0; i < masterList.numResourceEntries; i++)
        {
            std::string output = "";
            output += masterList.resourceList[i].name + ",";
            output += masterList.resourceList[i].type + ",";
            output += std::to_string(masterList.resourceList[i].hash) + ",";
            output += std::to_string(masterList.resourceList[i].version) + ",";
            output += std::to_string(masterList.resourceList[i].priority) + ",";
            output += std::to_string(masterList.resourceList[i].havokFlag1) + ",";
            output += std::to_string(masterList.resourceList[i].havokFlag2) + ",";
            output += std::to_string(masterList.resourceList[i].havokFlag3) + "\n";
            outputFile.write(output.c_str(), output.length());
        }
        printf("Write completed successfully. Parse complete.\n");
        return;
    }

    // build initial resource list from files
    void ResourceParser::buildResourceList()
    {
        for (int i = 0; i < (config.resourceFileNames.size()); i++)
        {
            std::string filename = config.resourceFileNames[i];
            ResourceFile resourceFile(filename, i);
            masterList.appendResourceEntries(resourceFile.resourceEntries);
        }
        return;
    }

    // main parse function
    void ResourceParser::parseResourceFiles(std::vector<std::string> inputFileList, char* logfile)
    {
        config.resourceFileNames = inputFileList;

        buildResourceList();
        printf("Finished parsing resource files. Found %d entries in %d files. \n", (int)masterList.numResourceEntries, (int)config.resourceFileNames.size());

        sortAndRemoveDuplicates();
        printf("Duplicates removed successfully. %d entries remaining. \n", (int)masterList.numResourceEntries);

        writeResourceListToCSV(logfile);
        return;
    }
}