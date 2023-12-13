#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <cstdlib>
#include <csignal>
#include <syslog.h>

void signalHandler(int signum);

void openLog();

#endif //AUDIT_LOGGER_H
