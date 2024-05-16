#ifndef IFILEMANAGEMENT_H
#define IFILEMANAGEMENT_H

#include <string>
#include <vector>

class IFileManagement {
public:
    virtual ~IFileManagement() {}

    virtual std::vector<std::string> getAllFiles() const = 0;
    virtual std::vector<std::string> readFile(const std::string& filePath) const = 0;
    virtual void clearFiles(const std::string& dirPath, const std::vector<std::string>& fileNames) = 0;
    virtual void writeFile(const std::string& filePath, const std::string& content, bool append = true) = 0;
    virtual void createEmptyFile(const std::string& filePath) = 0;

    virtual std::string getTempDirectory() const = 0;
    virtual std::string getOutputDirectory() const = 0;
};

#endif // IFILEMANAGEMENT_H