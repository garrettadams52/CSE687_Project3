#ifndef I_MAP_H
#define I_MAP_H

#include <string>

class IMap {
public:
    virtual void MapFunction(const std::string& filename, const std::string& content) = 0;
    virtual ~IMap() {} 
};

#endif // I_MAP_H

