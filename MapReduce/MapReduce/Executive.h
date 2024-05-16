#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "Workflow.h"
#include <string>
#include "Windows.h"

typedef IMap* (*CREATEMAPFUNC)(FileManagement&, int);
typedef IReduce* (*CREATEREDUCEFUNC)(FileManagement&);

class Executive {
    Workflow workflow;
    FileManagement fileManagement;
    HMODULE hMapDll;
    HMODULE hReduceDll;
    IMap* mapInstance;
    IReduce* reduceInstance;
    int bufferSize;  

public:
    Executive(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
        const std::string& mapDllPath, const std::string& reduceDllPath, int bufSize);
    ~Executive();
    void run();
    void markSuccess();
private:
    void loadMapDll(const std::string& path);
    void loadReduceDll(const std::string& path);
};

#endif 