

bin_PROGRAMS=srip

srip_CPPFLAGS=$(JSON_CFLAGS) $(JSONGLIB_CFLAGS) $(GLIB_CFLAGS) $(GIOUNIX_CFLAGS) $(DBUS_CFLAGS) $(GST_CFLAGS)
srip_LDFLAGS=$(JSON_LIBS) $(JSONGLIB_LIBS) $(GLIB_LIBS) $(GIOUNIX_LIBS) $(DBUS_LIBS) $(GST_LIBS)
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
            mpris.c \
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

