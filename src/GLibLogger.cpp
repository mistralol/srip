
#include "main.h"

GLibLogger::GLibLogger() {
    m_handle = g_log_set_handler(NULL, G_LOG_LEVEL_MASK, GLibLogHandler, NULL);
}

GLibLogger::~GLibLogger() {
    g_log_remove_handler(NULL, m_handle);
}

void GLibLogger::GLibLogHandler(const gchar *domain, GLogLevelFlags level, const gchar *msg, gpointer data) {
    switch(level) {
        case G_LOG_LEVEL_DEBUG:
                LogDebug("GLIB: %s", msg);
                break;
        case G_LOG_LEVEL_INFO:
                LogInfo("GLIB: %s", msg);
                break;
        case G_LOG_LEVEL_MESSAGE:
                LogNotice("GLIB: %s", msg);
                break;
        case G_LOG_LEVEL_WARNING:
                LogWarning("GLIB: %s", msg);
                break;
        case G_LOG_LEVEL_CRITICAL:
                LogCritical("GLIB: %s", msg);
                break;
        case G_LOG_LEVEL_ERROR:
                LogError("GLIB: %s", msg);
                break;
        case G_LOG_FLAG_RECURSION:
                LogCritical("GLIB: G_LOG_FLAG_RECURSION %d Message: %s", level, msg);
                abort();
                break;
        case G_LOG_FLAG_FATAL:
                LogCritical("GLIB-FATAL: Message: %s", msg);
                abort();
                break;
        case G_LOG_LEVEL_MASK:
                LogInfo("GLIB: %s", msg);
                break;
        default:
                LogCritical("GLIB: Unknown Log Level %d Message: %s", level, msg);
                abort();
                break;
    }
}
