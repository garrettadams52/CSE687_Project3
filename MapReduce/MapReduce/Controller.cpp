#include "Controller.h"
#include <windows.h>
#include <iostream>
#include <string>

void Controller::runMapperProcess(const std::string& exePath, const std::string& args) {
    std::wstring wExePath = std::wstring(exePath.begin(), exePath.end());
    std::wstring wArgs = std::wstring(args.begin(), args.end());
    std::wstring command = wExePath + L" " + wArgs;

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcess(NULL, &command[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cout << "Started process: " << exePath << " with args: " << args << std::endl;
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        std::cout << "Process completed: " << exePath << std::endl;
    }
    else {
        std::cerr << "Failed to start process: " << exePath << std::endl;
    }
}

void Controller::runMappers(int numMappers, const std::string& inputDir, const std::string& tempDir, const std::string& mapDllPath) {
    for (int i = 0; i < numMappers; ++i) {
        std::string args = "mapper " + std::to_string(i) + " " + inputDir + " " + tempDir + " " + mapDllPath;
        runMapperProcess("MapReduce.exe", args);
    }
}
