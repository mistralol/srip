
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include <list>
#include <string>
#include <sstream>
#include <functional>
#include <memory>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

#include "Time.h"
#include "Mutex.h"
#include "ScopedLock.h"
#include "GstLogger.h"
#include "GLibLogger.h"
#include "PipelineBasic.h"
#include "PulseSource.h"
#include "DBUSMedia.h"
#include "IOutputPipeline.h"
#include "OutputManager.h"
#include "OutputWav.h"
#include "OutputLame.h"
#include "OutputScopeBasic.h"

extern int verbose;

void Logger(int Type, const char *fmt, ...);

#define LogDebug(fmt, ...) Logger(LOG_DEBUG, fmt,##__VA_ARGS__ )
#define LogInfo(fmt, ...) Logger(LOG_INFO, fmt,##__VA_ARGS__)
#define LogNotice(fmt, ...) Logger(LOG_NOTICE, fmt,##__VA_ARGS__)
#define LogError(fmt, ...) Logger(LOG_ERR, fmt,##__VA_ARGS__)
#define LogCritical(fmt, ...) Logger(LOG_CRIT, fmt,##__VA_ARGS__)
#define LogWarning(fmt, ...) Logger(LOG_WARNING, fmt,##__VA_ARGS__)
#define LogAlert(fmt, ...) Logger(LOG_ALERT, fmt,##__VA_ARGS__)
