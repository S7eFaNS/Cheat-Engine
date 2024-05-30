#pragma once
#include <vector>
#include <string>
#include <Windows.h>

using PID = DWORD;
using PROCESS = HANDLE;

std::vector<std::pair<std::string, PID>> getAllProcesses();

bool isProcessAlive(PROCESS process);

PROCESS openProcess(PID pid);

void closeProcess(PROCESS process);


//start of scanning the memory
struct OppenedQuery
{
	PROCESS queriedProcess = 0;
	char *baseQueriedPtr = (char *)0x0;
	bool oppened() 
	{ 
		return queriedProcess != 0; 
	}
};

OppenedQuery initVirtualQuery(PROCESS process); 

enum
{
	memQueryFlags_ = 0,
	memQueryFlags_None = 0,
	memQueryFlags_Read = 0b0001,
	memQueryFlags_Write = 0b0010,
	memQueryFlags_Execute = 0b0100,
};

bool getNextQuery(OppenedQuery &query, void *&low, void *&hi, int &flags);

void writeMemory(PROCESS process, void* ptr, void* data, size_t size);

bool readMemory(PROCESS process, void *start, size_t size, void *buff); 

std::vector<void *> findBytePatternInProcessMemory(PROCESS process, void *pattern, size_t patternLen);

void refindBytePatternInProcessMemory(PROCESS process, void* pattern, size_t patternLen, std::vector<void*>& found);