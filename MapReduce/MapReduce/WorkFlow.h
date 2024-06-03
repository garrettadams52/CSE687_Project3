#ifndef WORKFLOW_H
#define WORKFLOW_H

#include "IMap.h"
#include "IReduce.h"
#include "FileManagement.h"
#include <string>
#include <vector>

class Workflow {
    std::string inputDirectory;
    std::string tempDirectory;
    std::string outputDirectory;
    std::string reduceDllPath;
    IMap* mapInstance;
    IReduce* reduceInstance;
    FileManagement fileManagement;

public:
    Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
        const std::string& reduceDllPath, IMap* map, IReduce* reduce);
    void run();
    static int partitionFunction(const std::string& key, int numReducers);
};

#endif 
