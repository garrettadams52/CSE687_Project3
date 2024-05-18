#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "Workflow.h"
#include <string>
#include <vector>
#include "Windows.h"

typedef IMap* (*CREATEMAPFUNC)(FileManagement*, int, int);
typedef IReduce* (*CREATEREDUCEFUNC)(FileManagement*);

class Executive {
    Workflow workflow;
    FileManagement fileManagement;
    HMODULE hMapDll;
    HMODULE hReduceDll;
    std::vector<IMap*> mapInstances; // Vector to store multiple mapper instances
    IReduce* reduceInstance;
    int bufferSize;
    int numReducers;

public:
    Executive(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
        const std::string& mapDllPath, const std::string& reduceDllPath, int bufSize, int numReducers);
    ~Executive();
    void run();
    void markSuccess();
private:
    void loadMapDll(const std::string& path);
    void loadReduceDll(const std::string& path);
};

#endif // EXECUTIVE_H
