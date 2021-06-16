#include "ResourceFile.h"

namespace ResourceToCSV
{
    // Helper functions for organization purposes - not meant to be called individually
    void ResourceFile::readFileHeader(FILE* f)
    {
        byte buff4[4];
        byte buff8[8];

        fseek(f, 32, SEEK_SET);
        fread(buff4, 4, 1, f);
        numFileEntries = *(int*)buff4;

        fread(buff4, 4, 1, f);
        numMetaEntries = *(uint32*)buff4;

        fread(buff4, 4, 1, f);
        numMetaIndexes = *(uint32*)buff4;

        fread(buff4, 4, 1, f);
        numPathStringIndexes = *(uint32*)buff4;

        fseek(f, 56, SEEK_SET);
        fread(buff4, 4, 1, f);
        sizeStrings = *(uint32*)buff4;

        fseek(f, 64, SEEK_SET);
        fread(buff8, 8, 1, f);
        addrStringOffsets = *(uint64*)buff8;

        fseek(f, 80, SEEK_SET);
        fread(buff8, 8, 1, f);
        addrEntries = *(uint64*)buff8;

        fread(buff8, 8, 1, f);
        addrMetaEntries = *(uint64*)buff8;

        fread(buff8, 8, 1, f);
        addrPaths = *(uint64*)buff8;

        return;
    }
    void ResourceFile::readStringOffsets(FILE* f)
    {
        // Move pointer to string offsets
        byte buff8[8];
        fseek(f, addrStringOffsets, SEEK_SET);
        fread(buff8, 8, 1, f);

        // Calculate strings start address
        numStrings = *(uint64*)buff8;
        addrStrings = addrStringOffsets + (numStrings * sizeof(uint64));

        // Read string offsets
        offsets.resize(numStrings);
        for (int i = 0; i < numStrings; i++)
        {
            fread(buff8, 8, 1, f);
            offsets[i] = *(uint64*)buff8;
        }
        return;
    }
    void ResourceFile::readPathIndexes(FILE* f)
    {
        // Move pointer to path indexes
        byte buff8[8];
        addrPathIndexes = addrPaths + (numMetaIndexes * sizeof(int));
        fseek(f, addrPathIndexes, SEEK_SET);

        // Read path indexes
        for (int i = 0; i < numPathStringIndexes; i++)
        {
            fread(buff8, 8, 1, f);
            pathStringIndexes.push_back(*(uint64*)buff8);
        }
        return;
    }
    void ResourceFile::readEntryData(FILE* f)
    {
        // Read file entries
        resourceEntries.resize(numFileEntries);
        for (uint64 i = 0; i < numFileEntries; i++)
        {
            byte buff1[1];
            byte buff4[4];
            byte buff8[8];
            char strbuff[512];
            uint64 thisEntryAddr = 0;
            uint64 pathTupleIndex = 0;
            uint64 offsetNumber = 0;
            uint64 stringOffset = 0;
            uint64 nextStringOffset = 0;
            uint64 thisStringLength = 0;
            const uint64 entrySizeInBytes = 144;

            thisEntryAddr = addrEntries + (i * entrySizeInBytes);
            fseek(f, thisEntryAddr, SEEK_SET);

            // get path index (for finding string name and type)
            fseek(f, 32, SEEK_CUR);
            fread(buff8, 8, 1, f);
            pathTupleIndex = *(uint64*)buff8;

            // get hash and version
            fseek(f, 56, SEEK_CUR);
            fread(buff8, 8, 1, f);
            resourceEntries[i].hash = *(uint64*)buff8;

            fread(buff4, 4, 1, f);
            resourceEntries[i].version = *(uint32*)buff4;

            // get new unknowns for havok stuff
            fread(buff4, 4, 1, f);
            resourceEntries[i].havokFlag1 = *(uint32*)buff4;

            fseek(f, 2, SEEK_CUR);
            fread(buff1, 1, 1, f);
            resourceEntries[i].havokFlag2 = *(byte*)buff1;
            fread(buff1, 1, 1, f);
            resourceEntries[i].havokFlag3 = *(byte*)buff1;

            // get string file type
            offsetNumber = pathStringIndexes[pathTupleIndex];
            stringOffset = addrStrings + offsets[offsetNumber] + 8;
            nextStringOffset = addrStrings + offsets[offsetNumber + 1] + 8;
            thisStringLength = nextStringOffset - stringOffset;

            fseek(f, stringOffset, SEEK_SET);
            fread(strbuff, 1, thisStringLength, f);
            resourceEntries[i].type = std::string(&strbuff[0], &strbuff[thisStringLength - 1]);

            // get string file name
            offsetNumber = pathStringIndexes[pathTupleIndex + 1];
            stringOffset = addrStrings + offsets[offsetNumber] + 8;

            // catch for last file entry
            if (offsetNumber + 1 == numStrings)
                nextStringOffset = addrMetaEntries;
            else
                nextStringOffset = addrStrings + offsets[offsetNumber + 1] + 8;
            thisStringLength = nextStringOffset - stringOffset;

            fseek(f, stringOffset, SEEK_SET);
            fread(strbuff, 1, thisStringLength, f);
            resourceEntries[i].name = "\"" + std::string(strbuff) + "\"";
            resourceEntries[i].priority = loadPriority;
        }
        return;
    }

    // Constructor, calls the helper functions above
    ResourceFile::ResourceFile(std::string filename, int loadPriority)
    {
        FILE* f;
        printf("Reading file %s\n", filename.c_str());
        if (fopen_s(&f, filename.c_str(), "rb") != 0) {
            printf("Error: failed to open %s for reading.\n", filename.c_str());
            return;
        }

        if (f != NULL)
        {
            this->loadPriority = loadPriority;
            readFileHeader(f);
            readStringOffsets(f);
            readPathIndexes(f);
            readEntryData(f);
        }
        fclose(f);
    }
}