

bin_PROGRAMS=srip

srip_CPPFLAGS=$(GST_CFLAGS) $(DBUS_CFLAGS)
srip_LDFLAGS=$(GST_LIBS) $(DBUS_LIBS)
srip_SOURCES=Mutex.cpp \
            ScopedLock.cpp \
            Time.cpp \
            GLibLogger.cpp \
            GstLogger.cpp \
            PipelineBasic.cpp \
            PulseSource.cpp \
            OutputManager.cpp \
            OutputWav.cpp \
            OutputLame.cpp \
            OutputScopeBasic.cpp \
            DBUSMedia.cpp \
            main.cpp

noinst_HEADERS=main.h \
            DBUSMedia.h \
            GstLogger.h \
            OutputLame.h \
            OutputScopeBasic.h \
            PipelineBasic.h \
            ScopedLock.h \
            GLibLogger.h \
            IOutputPipeline.h \
            Mutex.h \
            OutputManager.h \
            OutputWav.h \
            PulseSource.h \
            Time.h

