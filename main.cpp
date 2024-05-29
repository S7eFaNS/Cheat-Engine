#include <iostream>
#include "include/systemFunctions.h"


int main() {
    std::cout << "Welcome to cheat engine!\n";
    std::cout << "Press any button to display all processes.\n";
    std::cin.get();

    std::vector<std::pair<std::string, PID>> processes = getAllProcesses();

    std::cout << "Running processes:\n";
    for (const auto& process : processes)
    {
        std::cout << "Process Name: " << process.first << " | PID: " << process.second << "\n";
    }

    std::cout << "Please enter the PID of the game you want to hack: ";
    PID pid;
    std::cin >> pid;

    return 0;
}