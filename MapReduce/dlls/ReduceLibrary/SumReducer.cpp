#include "SumReducer.h"
#include <iostream>
#include <sstream>
#include <algorithm>

SumReducer::SumReducer(IFileManagement* fileManager) : fileManager(fileManager) {
    outputPath = fileManager->getOutputDirectory() + "/final_output.txt";
}

void SumReducer::reduce() {
    std::string inputPath = fileManager->getTempDirectory() + "/sorted_aggregated_output.txt";

    std::vector<std::string> lines = fileManager->readFile(inputPath);

    for (const std::string& line : lines) {
        std::istringstream iss(line);
        std::string key;
        char discard;
        std::vector<int> values;

        iss >> discard; 
        getline(iss, key, ',');
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());

        if (key.empty()) {
            std::cerr << "Found an empty key in line: " << line << std::endl;
            continue;
        }

        iss >> discard; 
        int num;
        while (iss >> num) {
            values.push_back(num);
            iss >> discard; 
        }

        int sum = 0;
        for (int value : values) {
            sum += value;
        }

        exportResult(key, sum);
    }
}

void SumReducer::exportResult(const std::string& key, int result) {
    std::string resultLine = "(" + key + ", " + std::to_string(result) + ")\n";
    fileManager->writeFile(outputPath, resultLine);
}

extern "C" REDUCELIBRARY_API IReduce * CreateReduceInstance(IFileManagement * fileManager) {
    return new SumReducer(fileManager);
}

