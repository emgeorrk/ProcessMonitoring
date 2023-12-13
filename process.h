#ifndef AUDIT_PROCESS_H
#define AUDIT_PROCESS_H

#include <string>
#include <utility>

class Process {
public:
    const std::string pid;
    const std::string user;
    const std::string name;
    const std::string command;
    const unsigned uptime{};

    Process() = default;

    Process(std::string pid, std::string user, std::string name, std::string command, unsigned uptime) :
            pid(std::move(pid)),
            user(std::move(user)),
            name(std::move(name)),
            command(std::move(command)),
            uptime(uptime){
        // pass
    };
};

#endif //AUDIT_PROCESS_H
