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

    std::string fullPath = tempDirectory + "\\temp_output.txt";
    outputFile.open(fullPath, std::ios::app);
    if (!outputFile) {
        throw std::runtime_error("Failed to open output file at: " + fullPath);
    }
}

WordCountMapper::~WordCountMapper() {
    flushBuffer();
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

void WordCountMapper::map(const std::string& fileName, const std::string& content) {
    std::istringstream stream(content);
    std::string word;
    while (stream >> word) {
        word.erase(std::remove_if(word.begin(), word.end(),
            [](unsigned char c) { return !std::isalpha(c); }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) {
            exportToFile(word);
        }
    }
}

void WordCountMapper::exportToFile(const std::string& word) {
    buffer.push_back("(" + word + ", 1)");
    if (buffer.size() >= bufferSize) {
        flushBuffer();
    }
}

void WordCountMapper::flushBuffer() {
    for (const auto& entry : buffer) {
        outputFile << entry << std::endl;
    }
    buffer.clear();
}


extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize) {
    return new WordCountMapper(fileManager, bufferSize);
}


