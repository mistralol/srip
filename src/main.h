
#include <syslog.h>

extern int verbose;

void Logger(int Type, const char *fmt, ...);

#define LogDebug(fmt, ...) Logger(LOG_DEBUG, fmt,##__VA_ARGS__ )
#define LogInfo(fmt, ...) Logger(LOG_INFO, fmt,##__VA_ARGS__)
#define LogError(fmt, ...) Logger(LOG_ERR, fmt,##__VA_ARGS__)
#define LogCritical(fmt, ...) Logger(LOG_CRIT, fmt,##__VA_ARGS__)
#define LogWarning(fmt, ...) Logger(LOG_WARNING, fmt,##__VA_ARGS__)
#define LogAlert(fmt, ...) Logger(LOG_ALERT, fmt,##__VA_ARGS__)
