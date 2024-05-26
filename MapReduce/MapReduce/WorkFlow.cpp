#include "Workflow.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Workflow::Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
    const std::vector<IMap*>& mapInstances, IReduce* reduceInstance)
    : inputDirectory(inputDir), tempDirectory(tempDir), outputDirectory(outputDir),
    mapInstances(mapInstances), reduceInstance(reduceInstance), fileManagement(inputDir, tempDir, outputDir) {}

void Workflow::runMap(int index) {
    // Implement your map execution logic here.
    // This is just an example.
    std::string fileName = inputDirectory + "\\temp_output_"; // Adjust this as needed
    std::vector<std::string> lines = fileManagement.readFile(fileName);
    for (const auto& line : lines) {
        mapInstances[index]->MapFunction(fileName, line);
    }
}

void Workflow::sortAndAggregate() {
    // Implement your sorting and aggregation logic here.
}

void Workflow::runReduce() {
    // Implement your reduce execution logic here.
}
