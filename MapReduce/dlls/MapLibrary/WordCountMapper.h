#ifndef WORD_COUNT_MAPPER_H
#define WORD_COUNT_MAPPER_H

#include "IFileManagement.h"
#include "IMap.h"
#include <vector>
#include <fstream>
#include <string>
#include <winsock2.h>

#ifdef MAPLIBRARY_EXPORTS
#define MAPLIBRARY_API __declspec(dllexport)
#else
#define MAPLIBRARY_API __declspec(dllimport)
#endif

class MAPLIBRARY_API WordCountMapper : public IMap {
    IFileManagement* fileManager;
    std::ofstream outputFile;
    std::vector<std::string> buffer;
    size_t bufferSize;
    SOCKET sendSocket;

public:
    WordCountMapper(IFileManagement* fileManager, size_t bufferSize);
    virtual ~WordCountMapper();

    void map(const std::string& fileName, const std::string& content);
    virtual void MapFunction(const std::string& filename, const std::string& content) override {
        map(filename, content);
    }
    void exportToFile(const std::string& word);
    void flushBuffer();

private:
    void initSocket();
    void sendHeartbeats();
};

extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize);

#endif // WORD_COUNT_MAPPER_H
