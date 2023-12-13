#include "logger.h"

void signalHandler(int signum) {
    syslog(LOG_NOTICE, "AuditDaemon stopped");
    closelog();
    exit(signum);
}

void openLog() {
    for (int signum = 0; signum < NSIG; ++signum) {
        signal(signum, signalHandler);
    }

    openlog("AuditDaemon", LOG_PID, LOG_DAEMON);

    syslog(LOG_NOTICE, "AuditDaemon started");
}