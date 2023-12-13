#include <iostream>
#include <chrono>
#include <thread>

#include "getpids.h"
#include "logger.h"

constexpr unsigned CHECK_INTERVAL_MS = 2000;

std::vector<Process> getNewProcesses(const std::unordered_map<unsigned, Process> &old,
                                     const std::unordered_map<unsigned, Process> &now) {
    std::vector<Process> new_processes;
    for (auto &element: now) {
        if (!element.second.command.empty() &&
            (!old.contains(element.first) || old.at(element.first).uptime > element.second.uptime)) {
            new_processes.push_back(element.second);
        }
    }

    return new_processes;
}

std::vector<Process> getExpiredProcesses(const std::unordered_map<unsigned, Process> &old,
                                         const std::unordered_map<unsigned, Process> &now) {
    std::vector<Process> expired_processes;
    for (auto &element: old) {
        if (!now.contains(element.first) && element.second.uptime != 0) {
            expired_processes.push_back(element.second);
        }
    }

    return expired_processes;
}

inline std::string makeMessage(const Process &process, bool is_expired = false) {
    std::string result = (is_expired) ? "# Expired process:" : "# New process:";
    result += "\nName: " + process.name +
              "\nProcess ID: " + process.pid +
              "\nUser: " + process.user +
              "\nUptime: " + std::to_string(process.uptime) +
              "\nCommand: " + process.command +
              "\n---------------------------\n";
    return result;
}

int main() {
    openLog();

    std::unordered_map<unsigned, Process> processes = std::move(getpids());

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(CHECK_INTERVAL_MS));
        std::unordered_map<unsigned, Process> tmp = std::move(getpids());

        std::vector<Process> new_processes = std::move(getNewProcesses(processes, tmp));
        std::vector<Process> expired_processes = std::move(getExpiredProcesses(processes, tmp));

        for (const auto &newProcess: new_processes) {
            const std::string message = std::move(makeMessage(newProcess));
            std::cout << message;
            syslog(LOG_INFO, "%s", message.c_str());
        }

        for (const auto &expiredProcess: expired_processes) {
            const std::string message = std::move(makeMessage(expiredProcess, true));
            std::cout << message;
            syslog(LOG_INFO, "%s", message.c_str());
        }

        processes = std::move(tmp);
    }
}