#include <iostream>
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <locale>
#include <codecvt>
#include "systemFunctions.h"
#include <algorithm>

#undef min
#undef max


//returns a vector of all running processes

std::vector<std::pair<std::string, PID>> getAllProcesses() 
{
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

//checks if the process is still running or not

bool isProcessAlive(PROCESS process)
{
    DWORD exitCode = 0;

    BOOL code = GetExitCodeProcess(process, &exitCode);

    return code && (exitCode == STILL_ACTIVE);
}

PROCESS openProcess(PID pid)
{

    HANDLE handleToProcess = OpenProcess(

        PROCESS_VM_READ | 

        PROCESS_VM_WRITE |

        PROCESS_QUERY_INFORMATION |

        PROCESS_VM_OPERATION,
        0
        , pid);

        if((handleToProcess == INVALID_HANDLE_VALUE) || (handleToProcess == 0)) {
            handleToProcess = 0;
        }

        return handleToProcess;
}

void closeProcess(PROCESS process)
{
	CloseHandle(process);
}

OppenedQuery initVirtualQuery(PROCESS process)
{
	OppenedQuery q = {};

	q.queriedProcess = process;
	q.baseQueriedPtr = 0;
	return q;
}

bool getNextQuery(OppenedQuery &query, void *&low, void *&hi, int &flags)
{
    if (query.queriedProcess == 0) { return false; }

	flags = memQueryFlags_None;
	low = nullptr;
	hi = nullptr;

	MEMORY_BASIC_INFORMATION memInfo;

	bool rez = 0;
	while(true)
	{
		rez = VirtualQueryEx(query.queriedProcess, (void *)query.baseQueriedPtr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

		if (!rez) 
		{
			query = {};
			return false; 
		}

		query.baseQueriedPtr = (char *)memInfo.BaseAddress + memInfo.RegionSize;
		
		if (memInfo.State == MEM_COMMIT)
		{
			if (memInfo.Protect & PAGE_READONLY)
			{
				flags |= memQueryFlags_Read;
			}

			if (memInfo.Protect & PAGE_READWRITE)
			{
				flags |= (memQueryFlags_Read | memQueryFlags_Write);
			}

			if (memInfo.Protect & PAGE_EXECUTE)
			{
				flags |= memQueryFlags_Execute;
			}

			if (memInfo.Protect & PAGE_EXECUTE_READ)
			{
				flags |= (memQueryFlags_Execute | memQueryFlags_Read);
			}

			if (memInfo.Protect & PAGE_EXECUTE_READWRITE)
			{
				flags |= (memQueryFlags_Execute | memQueryFlags_Read | memQueryFlags_Write);
			}

			if (memInfo.Protect & PAGE_EXECUTE_WRITECOPY)
			{
				flags |= (memQueryFlags_Execute | memQueryFlags_Read);
			}

			if (memInfo.Protect & PAGE_WRITECOPY)
			{
				flags |= memQueryFlags_Read;
			}

			low = memInfo.BaseAddress;
			hi = (char *)memInfo.BaseAddress + memInfo.RegionSize;
			return true;
		}
	}
}

void writeMemory(PROCESS process, void* ptr, void* data, size_t size)
{
	BOOL writeSucceeded = WriteProcessMemory(
		process,
		ptr,
		data,
		size,
		NULL);
}

bool readMemory(PROCESS process, void *start, size_t size, void *buff)
{
	SIZE_T readSize = 0;
	return ReadProcessMemory(process, start, buff, size, &readSize);
}

std::vector<void *> findBytePatternInProcessMemory(PROCESS process, void *pattern, size_t patternLen)
{
	if (patternLen == 0) { return {}; }

	std::vector<void *> returnVec;
	returnVec.reserve(1000);

	auto query = initVirtualQuery(process);

	if (!query.oppened())
		return {};

	void *low = nullptr;
	void *hi = nullptr;
	int flags = memQueryFlags_None;

	while (getNextQuery(query, low, hi, flags))
	{
		if ((flags | memQueryFlags_Read) && (flags | memQueryFlags_Write))
		{
			size_t size = (char *)hi - (char *)low;
			char *localCopyContents = new char[size];
			if (
				readMemory(process, low, size, localCopyContents)
				)
			{
				char *cur = localCopyContents;
				size_t curPos = 0;
				while (curPos < size - patternLen + 1)
				{
					if (memcmp(cur, pattern, patternLen) == 0)
					{
						returnVec.push_back((char *)low + curPos);
					}
					curPos++;
					cur++;
				}
			}
			delete[] localCopyContents;
		}
	}
	return returnVec;
}

void refindBytePatternInProcessMemory(PROCESS process, void* pattern, size_t patternLen, std::vector<void*>& found) 
{
	if (patternLen == 0) { return; }

	auto newFound = findBytePatternInProcessMemory(process, pattern, patternLen);

	std::vector<void*> intersect;
	intersect.resize(std::min(found.size(), newFound.size()));

	std::set_intersection(found.begin(), found.end(),
		newFound.begin(), newFound.end(),
		intersect.begin());

	intersect.erase(std::remove(intersect.begin(), intersect.end(), nullptr), intersect.end());

	found = std::move(intersect);
}
