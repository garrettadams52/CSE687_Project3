#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "Workflow.h"
#include <string>
#include <Windows.h>

typedef IMap* (*CREATEMAPFUNC)(FileManagement&, size_t, int, int);
typedef IReduce* (*CREATEREDUCEFUNC)(FileManagement&);

class Executive {
    Workflow workflow;
    FileManagement fileManagement;
    HMODULE hMapDll;
    HMODULE hReduceDll;
    IMap* mapInstance;
    IReduce* reduceInstance;
    int bufferSize;
    int numReducers;

public:
    Executive(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
        const std::string& mapDllPath, const std::string& reduceDllPath, int bufSize, int reducers);
    ~Executive();
    void run(const std::string& mode, int mapperIndex = -1);
    void markSuccess();

private:
    void loadMapDll(const std::string& path);
    void loadReduceDll(const std::string& path);
};

#endif 
