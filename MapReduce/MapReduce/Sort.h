#ifndef SORT_H
#define SORT_H

#include <string>
#include <vector>
#include <map>
#include "FileManagement.h"


class Sort {
public:
    explicit Sort(FileManagement* fileManager);
    void sortAndAggregate();

private:
    FileManagement* fileManager;
    std::map<std::string, std::vector<int>> aggregateData(const std::vector<std::pair<std::string, int>>& data);
};

#endif 
