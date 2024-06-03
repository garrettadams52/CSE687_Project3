#ifndef WORKFLOW_H
#define WORKFLOW_H

#include "FileManagement.h"
#include "IMap.h"
#include "IReduce.h"
#include <string>
#include <vector>
#include <thread>

class Workflow {
    std::string inputDirectory;
    std::string tempDirectory;
    std::string outputDirectory;
    IMap* mapInstance;
    IReduce* reduceInstance;
    FileManagement fileManagement;

public:
    Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir, IMap* map, IReduce* reduce);
    void run();
   
};

#endif // WORKFLOW_H
