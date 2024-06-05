#include <iostream>
#include <string>
#include <thread>
#include <windows.h>

const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\mapreduce_pipe";

int main() {
    HANDLE pipe = CreateFile(
        PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to connect to named pipe, GLE=" << GetLastError() << std::endl;
        return 1;
    }

    while (true) {
        std::string inputLine;
        std::cout << "Enter command (map/reduce) and parameters:" << std::endl;
        std::getline(std::cin, inputLine);

        if (inputLine == "exit") {
            break;
        }

        DWORD bytesWritten;
        BOOL success = WriteFile(pipe, inputLine.c_str(), static_cast<DWORD>(inputLine.size()), &bytesWritten, NULL);
        if (!success) {
            std::cerr << "WriteFile to pipe failed, GLE=" << GetLastError() << std::endl;
            break;
        }

        std::cout << "Message sent to stub process: " << inputLine << std::endl;
    }

    CloseHandle(pipe);

    return 0;
}
