#include <iostream>
#include "systemFunctions.h"

int main() {
    PID pid = 1472;

    std::cout << "Welcome to cheat engine!\n";
    std::cout << "----------------------------------\n";
    std::cout << "Press ENTER to display all processes.\n";
    std::cin.get();

    std::vector<std::pair<std::string, PID>> processes = getAllProcesses();

    std::cout << "Running processes:\n";
    for (const auto& process : processes)
    {
         std::cout << "Process Name: " << process.first << " | PID: " << process.second << "\n";
    }

    std::cout << "----------------------------------\n";

    std::cout << "Please enter the PID of the game you want to hack: ";
    std::cin >> pid;

    std::cout << "----------------------------------\n";

    PROCESS process = openProcess(pid);

    while (process == 0)
    {
        std::cout << "Failed to open the process. Please enter a new process ID: ";
        std:: cin >> pid;
        process = openProcess(pid);
    }

    std::cout << "Process opened successfully.\n";

    std::cout << "----------------------------------\n";

    int value;
    std::cout << "Please enter the VALUE you want to hack: ";
    std::cin >> value; 

    unsigned char pattern[sizeof(value)];
    memcpy(pattern, &value, sizeof(value));

    std::vector<void*> addresses = findBytePatternInProcessMemory(process, pattern, sizeof(value));

    std::cout << "----------------------------------\n";

    for (const auto& addr : addresses) {
        std::cout << "Address: " << addr << std::endl;
    }

    std::cout << "----------------------------------\n";

    std::cout << "Addresses found: " << addresses.size() << std::endl;

    std::cout << "----------------------------------\n";
    
    bool continueScanning = true;

    while (continueScanning) {
    std::cout << "Please enter the NEW VALUE you want to hack: ";
    std::cin >> value;

    unsigned char newPattern[sizeof(value)];
    memcpy(newPattern, &value, sizeof(value));

    refindBytePatternInProcessMemory(process, newPattern, sizeof(value), addresses);

    std::cout << "Addresses found:\n";
    for (const auto& addr : addresses) {
        std::cout << "Address: " << addr << std::endl;
    }
    std::cout << "----------------------------------\n";

    std::cout << "Total Addresses found: " << addresses.size() << std::endl;

    std::cout << "Do you want to scan again with a new value? (1 for yes, 0 for no): ";
    int choice;
    std::cin >> choice;
    continueScanning = (choice == 1);
}

    std::cout << "----------------------------------\n";

    std::cout << "Enter the address to write to: ";
    void* addr;
    std::cin >> addr;
    
    std::cout << "Enter the value to write: ";
    std::cin >> value;

    std::cout << "----------------------------------\n";

    writeMemory(process, addr, &value, sizeof(value));

    std::cout << "Memory written successfully.\n";
    std::cout << "Press ENTER to end program.\n";
    std::cin.get();

    closeProcess(process);

    std::cout << "Process closed successfully.\n";

    return 0;
}