#ifndef REDUCE_H
#define REDUCE_H

#include <string>
#include <vector>
#include <map>
#include "FileManagement.h"

class Reduce {
public:
    explicit Reduce(FileManagement* fileManager);
    void reduce();
private:
    FileManagement* fileManager;
    std::string outputPath;  
    void exportResult(const std::string& key, int result);
};

#endif // REDUCE_H
