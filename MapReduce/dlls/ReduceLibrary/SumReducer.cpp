#include "SumReducer.h"
#include <iostream>
#include <sstream>
#include <algorithm>

SumReducer::SumReducer(IFileManagement* fileManager, const std::string& inputFileName, const std::string& outputFileName) : fileManager(fileManager) {
    this->outputFileName = outputFileName;
    this->inputFileName = inputFileName;
    outputPath = fileManager->getOutputDirectory() + outputFileName;
}

void SumReducer::reduce() {
    std::string inputPath = fileManager->getTempDirectory() + inputFileName;
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

void SumReducer::finalSort(std::string filePath1, std::string filePath2) {

    std::vector<std::string> iFile1 = fileManager->readFile(filePath1);
    std::vector<std::string> iFile2 = fileManager->readFile(filePath2);

    std::vector <std::string>keyVec1;
    std::vector <std::string>keyVec2;

    std::vector<int>valueVec1;
    std::vector<int>valueVec2;


    for (const std::string& line : iFile1) {
        std::istringstream iss(line);
        std::string key;
        char discard;

        iss >> discard;
        getline(iss, key, ',');
        keyVec1.push_back(key);

        std::string num;
        getline(iss, num, ')');
        valueVec1.push_back(std::stoi(num));

    }

    for (const std::string& line : iFile2) {
        std::istringstream iss(line);
        std::string key;
        char discard;

        iss >> discard;
        getline(iss, key, ',');
        keyVec2.push_back(key);

        std::string num;
        getline(iss, num, ')');
        valueVec2.push_back(std::stoi(num));

    }

    std::vector<bool>alreadyCopied1(keyVec1.size(), 0);
    std::vector<bool>alreadyCopied2(keyVec2.size(), 0);

    int iter1 = 0;
    int iter2 = 0;

    while (iter1 < keyVec1.size() && iter2 < keyVec2.size()) {
        while (keyVec1[iter1] <= keyVec2[iter2]) {
            if (keyVec1[iter1] == keyVec2[iter2]) {
                int newVal = valueVec1[iter1] + valueVec2[iter2];
                exportResult(keyVec1[iter1], newVal);
                alreadyCopied2[iter2] = 1;
            }
            else {
                if (!alreadyCopied1[iter1]) {
                    exportResult(keyVec1[iter1], valueVec1[iter1]);
                }
            }
            iter1++;
            if (iter1 == keyVec1.size()) {
                break;
            }
        }
        while (keyVec2[iter2] <= keyVec1[iter1]) {
            if (keyVec1[iter1] == keyVec2[iter2]) {
                int newVal = valueVec2[iter2] + valueVec1[iter1];
                exportResult(keyVec2[iter2], newVal);
                alreadyCopied1[iter1] = 1;
            }
            else {
                if (!alreadyCopied2[iter2]) {
                    exportResult(keyVec2[iter2], valueVec2[iter2]);
                }
            }
            iter2++;
            if (iter2 == keyVec2.size()) {
                break;
            }
        }

    }

    while ((iter1 == keyVec1.size()) && (iter2 < keyVec2.size())) {
        exportResult(keyVec2[iter2], valueVec2[iter2]);
        iter2++;
    }

    while ((iter2 == keyVec2.size()) && (iter1 < keyVec1.size())) {
        exportResult(keyVec1[iter1], valueVec1[iter1]);
        iter1++;
    }

}


extern "C" REDUCELIBRARY_API IReduce * CreateReduceInstance(IFileManagement * fileManager, const std::string& inputFilePath, const std::string& outputFilePath) {
    return new SumReducer(fileManager, inputFilePath, outputFilePath);
}

