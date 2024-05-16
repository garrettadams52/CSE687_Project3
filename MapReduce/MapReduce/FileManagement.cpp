#include "FileManagement.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

FileManagement::FileManagement(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir)
    : inputDirectory(inputDir), tempDirectory(tempDir), outputDirectory(outputDir) {}

std::vector<std::string> FileManagement::getAllFiles() const {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(inputDirectory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

std::vector<std::string> FileManagement::readFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return {};
    }

    std::vector<std::string> lines;
    std::string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }


    file.close();
    return lines;
}


std::string FileManagement::getTempDirectory() const {
    return tempDirectory;
}

std::string FileManagement::getOutputDirectory() const {
    return outputDirectory;
}


void FileManagement::clearFiles(const std::string& dirPath, const std::vector<std::string>& fileNames) {
    try {
        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
            for (const auto& entry : fs::directory_iterator(dirPath)) {
                if (std::find(fileNames.begin(), fileNames.end(), entry.path().filename()) != fileNames.end()) {
                    fs::remove(entry);
                    std::cout << "Removed file: " << entry.path() << std::endl;
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error clearing specific files in directory " << dirPath << ": " << e.what() << std::endl;
    }
}

void FileManagement::writeFile(const std::string& filePath, const std::string& content, bool append) {
    std::ofstream file(filePath, append ? std::ios::app : std::ios::out);
    if (!file) {
        std::cerr << "Unable to open file for writing: " << filePath << std::endl;
        return;
    }
    file << content;
    file.close();
}

void FileManagement::createEmptyFile(const std::string& filePath) {
    std::ofstream file(filePath);
    file.close();
}
