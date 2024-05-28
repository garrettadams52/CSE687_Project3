#ifndef I_REDUCE_H
#define I_REDUCE_H

class IReduce {
public:
    virtual void ReduceFunction() = 0;
    virtual void finalSort(std::string filePath1, std::string filePath2) = 0;
    virtual ~IReduce() {} 
};

#endif 

