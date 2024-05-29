#pragma once
#include <vector>
#include <string>
#include <Windows.h>

using PID = DWORD;
using PROCESS = HANDLE;

std::vector<std::pair<std::string, PID>> getAllProcesses();