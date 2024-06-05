#include "SumReducer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

SumReducer::SumReducer(IFileManagement* fileManager, const std::string& inputFileName, const std::string& outputFileName)
    : fileManager(fileManager) {
    this->outputFileName = outputFileName;
    this->inputFileName = inputFileName;
    outputPath = fileManager->getOutputDirectory() + outputFileName;

    initSocket();
}

SumReducer::~SumReducer() {
    closesocket(sendSocket);
    WSACleanup();
}

void SumReducer::initSocket() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return;
    }

    struct addrinfo* addrResult = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo("127.0.0.1", "27015", &hints, &addrResult);
    if (result != 0) {
        std::cerr << "getaddrinfo failed with error: " << result << std::endl;
        WSACleanup();
        return;
    }

    sendSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (sendSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return;
    }

    result = connect(sendSocket, addrResult->ai_addr, static_cast<int>(addrResult->ai_addrlen));
    if (result == SOCKET_ERROR) {
        std::cerr << "Unable to connect to server: " << WSAGetLastError() << std::endl;
        closesocket(sendSocket);
        sendSocket = INVALID_SOCKET;
    }

    freeaddrinfo(addrResult);

    if (sendSocket == INVALID_SOCKET) {
        std::cerr << "Unable to connect to server!" << std::endl;
        WSACleanup();
        return;
    }

    std::thread(&SumReducer::sendHeartbeats, this).detach();
}

void SumReducer::sendHeartbeats() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::string message = "Reduce process heartbeat.";
        int result = send(sendSocket, message.c_str(), static_cast<int>(message.size()), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

void SumReducer::reduce() {
    std::string inputPath = fileManager->getTempDirectory() + inputFileName;
    std::vector<std::string> lines = fileManager->readFile(inputPath);

    for (const std::string& line : lines) {
        std::istringstream iss(line);
        std::string key;
        char discard;
        std::vector<int> values;

        iss >> discard;
        getline(iss, key, ',');
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());

        if (key.empty()) {
            std::cerr << "Found an empty key in line: " << line << std::endl;
            continue;
        }

        iss >> discard;
        int num;
        while (iss >> num) {
            values.push_back(num);
            iss >> discard;
        }

        int sum = 0;
        for (int value : values) {
            sum += value;
        }

        exportResult(key, sum);
    }
}

void SumReducer::exportResult(const std::string& key, int result) {
    std::string resultLine = "(" + key + ", " + std::to_string(result) + ")\n";
    fileManager->writeFile(outputPath, resultLine);
}

void SumReducer::finalSort(std::string filePath1, std::string filePath2) {
    std::vector<std::string> iFile1 = fileManager->readFile(filePath1);
    std::vector<std::string> iFile2 = fileManager->readFile(filePath2);

    std::vector <std::string> keyVec1;
    std::vector <std::string> keyVec2;

    std::vector<int> valueVec1;
    std::vector<int> valueVec2;

    for (const std::string& line : iFile1) {
        std::istringstream iss(line);
        std::string key;
        char discard;

        iss >> discard;
        getline(iss, key, ',');
        keyVec1.push_back(key);

        std::string num;
        getline(iss, num, ')');
        valueVec1.push_back(std::stoi(num));
    }

    for (const std::string& line : iFile2) {
        std::istringstream iss(line);
        std::string key;
        char discard;

        iss >> discard;
        getline(iss, key, ',');
        keyVec2.push_back(key);

        std::string num;
        getline(iss, num, ')');
        valueVec2.push_back(std::stoi(num));
    }

    std::vector<bool> alreadyCopied1(keyVec1.size(), 0);
    std::vector<bool> alreadyCopied2(keyVec2.size(), 0);

    int iter1 = 0;
    int iter2 = 0;

    while (iter1 < keyVec1.size() && iter2 < keyVec2.size()) {
        while (keyVec1[iter1] <= keyVec2[iter2]) {
            if (keyVec1[iter1] == keyVec2[iter2]) {
                int newVal = valueVec1[iter1] + valueVec2[iter2];
                exportResult(keyVec1[iter1], newVal);
                alreadyCopied2[iter2] = 1;
            }
            else {
                if (!alreadyCopied1[iter1]) {
                    exportResult(keyVec1[iter1], valueVec1[iter1]);
                }
            }
            iter1++;
            if (iter1 == keyVec1.size()) {
                break;
            }
        }
        while (keyVec2[iter2] <= keyVec1[iter1]) {
            if (keyVec1[iter1] == keyVec2[iter2]) {
                int newVal = valueVec2[iter2] + valueVec1[iter1];
                exportResult(keyVec2[iter2], newVal);
                alreadyCopied1[iter1] = 1;
            }
            else {
                if (!alreadyCopied2[iter2]) {
                    exportResult(keyVec2[iter2], valueVec2[iter2]);
                }
            }
            iter2++;
            if (iter2 == keyVec2.size()) {
                break;
            }
        }
    }

    while ((iter1 == keyVec1.size()) && (iter2 < keyVec2.size())) {
        exportResult(keyVec2[iter2], valueVec2[iter2]);
        iter2++;
    }

    while ((iter2 == keyVec2.size()) && (iter1 < keyVec1.size())) {
        exportResult(keyVec1[iter1], valueVec1[iter1]);
        iter1++;
    }
}

extern "C" REDUCELIBRARY_API IReduce * CreateReduceInstance(IFileManagement * fileManager, const std::string & inputFilePath, const std::string & outputFilePath) {
    return new SumReducer(fileManager, inputFilePath, outputFilePath);
}
