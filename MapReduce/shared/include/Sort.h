#ifndef SORT_H
#define SORT_H

#include <string>
#include <vector>
#include <map>
#include "FileManagement.h"


class Sort {
public:
    explicit Sort(FileManagement* fileManager, std::string inputDir1, std::string inputDir2);
    void sortAndAggregate();

private:
    FileManagement* fileManager;
    std::map<std::string, std::vector<int>> aggregateData(const std::vector<std::pair<std::string, int>>& data);
    std::string inputDir1;
    std::string inputDir2;
};

#endif 
