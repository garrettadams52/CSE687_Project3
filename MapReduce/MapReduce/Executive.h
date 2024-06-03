#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "FileManagement.h"
#include "Workflow.h"
#include "IMap.h"
#include "IReduce.h"
#include <string>
#include <Windows.h>

// Function pointers for creating instances from DLLs
typedef IMap* (*CREATEMAPFUNC)(IFileManagement* fileManager, size_t bufferSize);
typedef IReduce* (*CREATEREDUCEFUNC)(IFileManagement* fileManager);

class Executive {
    FileManagement fileManagement;
    Workflow workflow;
    HMODULE hMapDll;
    HMODULE hReduceDll;
    IMap* mapInstance;
    IReduce* reduceInstance;
    int bufferSize;
    int numReducers;

public:
    Executive(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
        const std::string& mapDllPath, const std::string& reduceDllPath, int bufSize, int numReducers);
    ~Executive();

    void run(const std::string& mode, int mapperIndex = 0);

private:
    void loadMapDll(const std::string& path);
    void loadReduceDll(const std::string& path);
    void markSuccess();
};

#endif // EXECUTIVE_H
