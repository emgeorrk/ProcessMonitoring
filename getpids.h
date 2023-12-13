#ifndef AUDIT_GETPIDS_H
#define AUDIT_GETPIDS_H

#include <unordered_map>

#include "process.h"

std::unordered_map<unsigned, Process> getpids();

#endif //AUDIT_GETPIDS_H
