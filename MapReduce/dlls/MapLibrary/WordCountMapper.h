#ifndef WORD_COUNT_MAPPER_H
#define WORD_COUNT_MAPPER_H

#include "IFileManagement.h"
#include "IMap.h"
#include "FileWriter.h"
#include <vector>
#include <string>
#include <utility> // for std::pair

#ifdef MAPLIBRARY_EXPORTS
#define MAPLIBRARY_API __declspec(dllexport)
#else
#define MAPLIBRARY_API __declspec(dllimport)
#endif

class MAPLIBRARY_API WordCountMapper : public IMap {
    IFileManagement* fileManager;                         // Pointer to file management interface
    std::vector<FileWriter*> outputFiles;                 // Vector of pointers to FileWriter for each reducer
    std::vector<std::pair<int, std::string>> buffer;      // Buffer to store partitioned key-value pairs
    size_t bufferSize;                                    // Size of the buffer before flushing
    int numReducers;                                      // Number of reducer processes

public:
    // Constructor: Initializes the mapper with file manager, buffer size, and number of reducers
    WordCountMapper(IFileManagement* fileManager, size_t bufferSize, int numReducers);

    // Destructor: Flushes the buffer and closes all output files
    virtual ~WordCountMapper();

    // Map function: Processes the content of the input file
    void map(const std::string& fileName, const std::string& content);

    // Overrides the MapFunction from IMap interface
    virtual void MapFunction(const std::string& filename, const std::string& content) override {
        map(filename, content);
    }

    // Exports the word to the appropriate output file based on partition
    void exportToFile(const std::string& word);

    // Flushes the buffer to the output files
    void flushBuffer();

    // Partition function: Determines the partition for a given key
    int partitionFunction(const std::string& key);
};

// Factory function: Creates an instance of the WordCountMapper
extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize, int numReducers);

#endif // WORD_COUNT_MAPPER_H
