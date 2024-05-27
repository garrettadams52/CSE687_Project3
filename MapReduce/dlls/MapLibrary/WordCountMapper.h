#ifndef WORD_COUNT_MAPPER_H
#define WORD_COUNT_MAPPER_H

#include "IFileManagement.h"
#include "IMap.h"
#include <vector>
#include <string>
#include <utility>
#include <fstream>

#ifdef MAPLIBRARY_EXPORTS
#define MAPLIBRARY_API __declspec(dllexport)
#else
#define MAPLIBRARY_API __declspec(dllimport)
#endif

class MAPLIBRARY_API WordCountMapper : public IMap {
    IFileManagement* fileManager;
    std::vector<std::ofstream*> outputFiles;
    std::vector<std::pair<int, std::string>> buffer;
    size_t bufferSize;
    int numReducers;

public:
    WordCountMapper(IFileManagement* fileManager, size_t bufferSize, int numReducers);
    virtual ~WordCountMapper();

    void map(const std::string& fileName, const std::string& content);
    virtual void MapFunction(const std::string& filename, const std::string& content) override {
        map(filename, content);
    }

    void exportToFile(const std::string& word);
    void flushBuffer();
    int partitionFunction(const std::string& key);
};

extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize, int numReducers);

#endif // WORD_COUNT_MAPPER_H
