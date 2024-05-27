#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>

class Controller {
public:
    void runMapperProcess(const std::string& exePath, const std::string& args);
    void runMappers(int numMappers, const std::string& inputDir, const std::string& tempDir, const std::string& mapDllPath);
};

#endif // CONTROLLER_H
