#ifndef WORD_COUNT_MAPPER_H
#define WORD_COUNT_MAPPER_H

#include "IFileManagement.h"
#include "IMap.h"
#include <vector>
#include <fstream>
#include <string>

#ifdef MAPLIBRARY_EXPORTS
#define MAPLIBRARY_API __declspec(dllexport)
#else
#define MAPLIBRARY_API __declspec(dllimport)
#endif

class MAPLIBRARY_API WordCountMapper : public IMap {
    IFileManagement* fileManager;
    size_t bufferSize;
    std::vector<std::string> buffer;
    std::ofstream outputFile;

public:
    WordCountMapper(IFileManagement* fileManager, size_t bufferSize);
    virtual ~WordCountMapper();

    void map(const std::string& fileName, const std::string& content);
    virtual void MapFunction(const std::string& filename, const std::string& content) override;

    void exportToFile(const std::string& word, int reducerIndex, int mapperIndex);
    void flushBuffer();

private:
    int partitionFunction(const std::string& key, int numReducers);
};

extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize);

#endif // WORD_COUNT_MAPPER_H
