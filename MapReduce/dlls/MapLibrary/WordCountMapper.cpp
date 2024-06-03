#include "WordCountMapper.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

WordCountMapper::WordCountMapper(IFileManagement* fileManager, size_t bufferSize)
    : fileManager(fileManager), bufferSize(bufferSize) {
    if (!fileManager) {
        throw std::invalid_argument("fileManager pointer cannot be null.");
    }

    std::string tempDirectory = fileManager->getTempDirectory();
    if (tempDirectory.empty()) {
        throw std::runtime_error("Temporary directory cannot be empty.");
    }

    if (!fs::exists(tempDirectory)) {
        fs::create_directories(tempDirectory);
    }
}

WordCountMapper::~WordCountMapper() {
    flushBuffer();
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

int WordCountMapper::partitionFunction(const std::string& key, int numReducers) {
    std::hash<std::string> hashFn;
    return hashFn(key) % numReducers;
}

void WordCountMapper::map(const std::string& fileName, const std::string& content) {
    std::istringstream stream(content);
    std::string word;
    while (stream >> word) {
        word.erase(std::remove_if(word.begin(), word.end(),
            [](unsigned char c) { return !std::isalpha(c); }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) {
            int reducerIndex = partitionFunction(word, 2); // Assuming 2 reducers
            int mapperIndex = 0; // Assuming mapperIndex is known
            exportToFile(word, reducerIndex, mapperIndex);
        }
    }
}

void WordCountMapper::MapFunction(const std::string& filename, const std::string& content) {
    map(filename, content);
}

void WordCountMapper::exportToFile(const std::string& word, int reducerIndex, int mapperIndex) {
    std::string outputFileName = fileManager->getTempDirectory() + "\\mapper_" + std::to_string(mapperIndex) + "_reducer_" + std::to_string(reducerIndex) + ".txt";
    buffer.push_back("(" + word + ", 1)");
    if (buffer.size() >= bufferSize) {
        flushBuffer();
    }
}

void WordCountMapper::flushBuffer() {
    if (buffer.empty()) return;

    if (!outputFile.is_open()) {
        std::string outputFileName = fileManager->getTempDirectory() + "\\temp_output.txt";
        outputFile.open(outputFileName, std::ios::app);
        if (!outputFile) {
            throw std::runtime_error("Failed to open output file at: " + outputFileName);
        }
    }

    for (const auto& entry : buffer) {
        outputFile << entry << std::endl;
    }
    buffer.clear();
}

extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize) {
    return new WordCountMapper(fileManager, bufferSize);
}
