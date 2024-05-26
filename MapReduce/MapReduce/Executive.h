#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "Workflow.h"
#include "FileManagement.h"
#include <string>
#include <vector>
#include <windows.h>

typedef IMap* (*CREATEMAPFUNC)(IFileManagement*, size_t, int);
typedef IReduce* (*CREATEREDUCEFUNC)(IFileManagement*);

class Executive {
    Workflow* workflow;
    FileManagement fileManagement;
    std::vector<HMODULE> hMapDlls;
    HMODULE hReduceDll;
    std::vector<IMap*> mapInstances;
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
    void loadMapDlls(const std::string& path);
    void loadReduceDll(const std::string& path);
};

#endif // EXECUTIVE_H
