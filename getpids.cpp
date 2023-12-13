#include <iostream>
#include <vector>
#include <unordered_map>

#include <fstream>
#include <sstream>
#include <dirent.h>
#include <filesystem>

#include "process.h"

// /proc/[PID]/status
std::string getProcessStatusValue(const std::string &pid, const std::string &key) {
    std::ifstream statusFile("/proc/" + pid + "/status");
    std::string line;

    if (!statusFile.is_open()) {
        std::cerr << "Ошибка открытия файла: /proc/" + pid + "/status"  << std::endl;
        return "";
    }

    while (std::getline(statusFile, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == key + ":") {
            std::string value;
            iss >> value;
            statusFile.close();
            return value;
        }
    }

    statusFile.close();
    return "";
}

unsigned getUptime(const std::string &pid) {
    std::ifstream statFile("/proc/" + pid + "/stat");
    std::string line, dummy;
    unsigned uptime;

    getline(statFile, line);
    statFile.close();

    std::istringstream iss(line);

    for (int i = 0; i < 13; ++i) {
        iss >> dummy;
    }
    iss >> uptime;

    return uptime;
}

std::unordered_map<unsigned, Process> getpids() {
    std::unordered_map<unsigned, Process> processes;

    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");

    if (dir == nullptr) {
        std::cerr << "Error opening /proc directory" << std::endl;
        exit(1);
    }

    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) {
            std::string pid = entry->d_name;
            unsigned pid_int = std::stoi(pid);

            std::string user = getProcessStatusValue(pid, "Uid");
            std::string name = getProcessStatusValue(pid, "Name");

            std::string command;
            std::ifstream cmdlineFile("/proc/" + pid + "/cmdline");
            std::getline(cmdlineFile, command);

            unsigned uptime = getUptime(pid);

            processes.emplace(pid_int, Process{pid, user, name, command, uptime});
        }
    }
    closedir(dir);
    return processes;
}
