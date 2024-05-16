#ifndef MAP_H
#define MAP_H

#include "FileManagement.h"
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>

class Map {
public:
    Map(FileManagement* fileManager, size_t bufferSize = 1000);
    ~Map();
    void map(const std::string& fileName, const std::string& content);
    void flushBuffer();  // Manually flush the buffer
    void exportToFile(const std::string& fileName, const std::string& word);  // Declare this method


private:
    FileManagement* fileManager;
    std::ofstream outputFile;
    size_t bufferSize;
    std::vector<std::string> buffer;  // Use a vector to buffer the output
};

#endif // MAP_H
