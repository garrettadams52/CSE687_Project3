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
    std::vector<IMap*> mapInstances; // Vector to store multiple mapper instances
    IReduce* reduceInstance;
    FileManagement fileManagement;

public:
    Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir, const std::vector<IMap*>& mappers, IReduce* reduce);
    void run();
};

#endif // WORKFLOW_H
