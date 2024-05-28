#include "Workflow.h"
#include <iostream>
#include <sstream>
#include <algorithm>



// Merged Constructor
Workflow::Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
    const std::vector<IMap*>& mapInstances, IReduce* reduceInstance)
    : inputDirectory(inputDir), tempDirectory(tempDir), outputDirectory(outputDir),
    mapInstances(mapInstances), reduceInstance(reduceInstance), fileManagement(inputDir, tempDir, outputDir) {}

// Function to run map instances
void Workflow::runMap(int index) {
    // Merged logic to ensure each mapper processes its files
    std::string fileName = inputDirectory + "\\temp_output_" + std::to_string(index); // Adjust this as needed
    std::vector<std::string> lines = fileManagement.readFile(fileName);
    for (const auto& line : lines) {
        mapInstances[index]->MapFunction(fileName, line);
    }
}

   

void Workflow::sortAndAggregate() {
    // Implement sorting and aggregation logic
}

void Workflow::runReduce() {
    // Implement reduce execution logic
}
