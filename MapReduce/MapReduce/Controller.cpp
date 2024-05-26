#include "Controller.h"
#include <windows.h>
#include <iostream>
#include <string>

void Controller::runMapperProcess(const std::string& exePath, const std::string& args) {
    std::wstring wExePath(exePath.begin(), exePath.end());
    std::wstring wArgs(args.begin(), args.end());
    std::wstring command = wExePath + L" " + wArgs;

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcess(NULL, &command[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        std::cerr << "Failed to start process: " << exePath << std::endl;
    }
}

void Controller::runMappers(int numMappers, const std::string& inputDir, const std::string& tempDir, const std::string& mapDllPath) {
    for (int i = 0; i < numMappers; ++i) {
        std::string args = std::to_string(i) + " " + inputDir + " " + tempDir + " " + mapDllPath;
        runMapperProcess("WordCountMapper.exe", args);
    }
}
