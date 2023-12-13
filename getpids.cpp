#include <iostream>
#include <vector>
#include <unordered_map>

#include <fstream>
#include <sstream>
#include <dirent.h>
#include <filesystem>

#include "getpids.h"
#include "process.h"
#include "logger.h"

// parsing file /proc/[PID]/status
std::string getProcessStatusValue(const std::string &pid, const std::string &key) {
    std::ifstream statusFile("/proc/" + pid + "/status");

    if (!statusFile.is_open()) {
        std::string err_message = "Error opening file: /proc/" + pid + "/status";
        std::cerr << err_message << std::endl;
        syslog(LOG_ERR, "%s", err_message.c_str());
        exit(1);
    }

    std::string line;

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

// parsing file /proc/[pid]/stat
unsigned getUptime(const std::string &pid) {
    std::ifstream statFile("/proc/" + pid + "/stat");

    if (!statFile.is_open()) {
        std::string err_message = "Error opening file: /proc/" + pid + "/stat";
        syslog(LOG_ERR, "%s", err_message.c_str());
        exit(1);
    }

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

// parsing processes from /proc
std::unordered_map<unsigned, Process> getpids() {
    std::unordered_map<unsigned, Process> processes;

    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");

    if (dir == nullptr) {
        std::string err_message = "Error opening directory: /proc";
        syslog(LOG_ERR, "%s", err_message.c_str());
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

            if (!cmdlineFile.is_open()) {
                std::string err_message = "Error opening file: /proc/" + pid + "/cmdline";
                std::cerr << err_message << std::endl;
                syslog(LOG_ERR, "%s", err_message.c_str());
            }

            std::getline(cmdlineFile, command);

            unsigned uptime = getUptime(pid);

            processes.emplace(pid_int, Process{pid, user, name, command, uptime});
        }
    }

    closedir(dir);
    return processes;
}
