#ifndef AUDIT_GETPIDS_H
#define AUDIT_GETPIDS_H

#include <unordered_map>

#include "process.h"

std::string getProcessStatusValue(const std::string &pid, const std::string &key);

unsigned getUptime(const std::string &pid);

std::unordered_map<unsigned, Process> getpids();

#endif //AUDIT_GETPIDS_H
