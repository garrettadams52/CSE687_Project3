// Reducer1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include "C:\Users\mark\source\repos\CIS687_Project3\CSE687_Project3\MapReduce\MapReduce\Executive.h"
#include "C:\Users\mark\source\repos\CIS687_Project3\CSE687_Project3\MapReduce\MapReduce\FileManagement.h"
#include "C:\Users\mark\source\repos\CIS687_Project3\CSE687_Project3\MapReduce\shared\include\IReduce.h"

namespace fs = std::filesystem;

int main()
{
	std::string inputDir = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\InputDir1";
	std::string tempDir = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\TempDir1";
	std::string outputDir = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\ReducedDir1";

	FileManagement manager(inputDir, tempDir, outputDir);

	HMODULE hReduceDll = LoadLibrary(L"C:\\Users\\mark\\source\\repos\\CIS687_Project3\\CSE687_Project3\\MapReduce\\x64\\Debug\\ReduceLibrary.dll");
	if (!hReduceDll) {
		std::cerr << "Failed to load Reduce DLL" << std::endl;
		//return -1;
	}

	std::string inFile = "\\sorted_aggregated_output1.txt";
	std::string outFile = "\\reduced_output1.txt";

	CREATEREDUCEFUNC createReduce= (CREATEREDUCEFUNC)GetProcAddress(hReduceDll, "CreateReduceInstance");
	if (!createReduce) {
		std::cerr << "Failed to find CreateReduceInstance function" << std::endl;
		//return -1;
	}

	IReduce* reducer = createReduce(manager, inFile, outFile);

	reducer->ReduceFunction();

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
