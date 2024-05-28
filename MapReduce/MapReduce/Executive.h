#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "Workflow.h"
#include "FileManagement.h"
#include <string>
#include <vector>
#include <windows.h>

// Typedefs for function pointers to the DLL functions
typedef IMap* (*CREATEMAPFUNC)(IFileManagement*, size_t, int, int); // Added mapperID to the signature
typedef IReduce* (*CREATEREDUCEFUNC)(IFileManagement*);

class Executive {
    Workflow* workflow; // Workflow instance
    FileManagement fileManagement; // File management instance
    std::vector<HMODULE> hMapDlls; // Vector to store map DLL handles
    HMODULE hReduceDll; // Handle for reduce DLL
    std::vector<IMap*> mapInstances; // Vector to store map instances
    IReduce* reduceInstance; // Pointer to reduce instance
    int bufferSize; // Buffer size for mappers
    int numReducers; // Number of reducers

public:
    // Constructor to initialize the Executive class with the provided parameters
    Executive(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
        const std::string& mapDllPath, const std::string& reduceDllPath, int bufSize, int numReducers);
    ~Executive(); // Destructor to clean up resources
    void run(); // Method to run the map and reduce processes
    void markSuccess(); // Method to mark the process as successful

private:
    void loadMapDlls(const std::string& path); // Method to load map DLLs and create map instances
    void loadReduceDll(const std::string& path); // Method to load reduce DLL and create reduce instance
};

#endif // EXECUTIVE_H
