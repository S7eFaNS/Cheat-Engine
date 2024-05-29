#include <iostream>
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <locale>
#include <codecvt>
#include "systemFunctions.h"



std::vector<std::pair<std::string, PID>> getAllProcesses() {
    HANDLE h;
    PROCESSENTRY32 singleProcess = {sizeof(PROCESSENTRY32)}; 

    h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    std::vector<std::pair<std::string, PID>> returnVector;
	returnVector.reserve(500);

    while (Process32Next(h, &singleProcess)) {
        if (singleProcess.th32ProcessID == 0) { continue; } //ignore system process

		std::pair<std::string, PID> process;

		process.first = singleProcess.szExeFile;
		process.second = singleProcess.th32ProcessID;

		returnVector.push_back(std::move(process));
    }

    CloseHandle(h);

    return returnVector;
}