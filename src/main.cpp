
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "main.h"

int verbose = 0;

static void print_help(FILE *fp, char *app) {
    fprintf(fp, "Usage: %s <options>\n", app);
    fprintf(fp, "\n");
    fprintf(fp, " -h --help           Print this help\n");
    fprintf(fp, " -d --debug          Switch on debug level logging\n");
    fprintf(fp, " -V --version        Print version and exit\n");
    fprintf(fp, "\n");
    fprintf(fp, "    --output-wav     Output to wav file\n");
    fprintf(fp, "    --output-lame    Output to mp3 file using lame encoder\n");
    fprintf(fp, "\n");
}

int main(int argc, char **argv) {
	const char *opts = "hdV";
    int longindex = 0;
    int c = 0;
    struct option loptions[] {
        {"help", 0, 0, 'h'},
        {"debug", 0, 0, 'd'},
        {"version", 0, 0, 'v'},
        {"output-wav", 0, 0, 0},
        {"output-lame", 0, 0, 0},
        {0, 0, 0, 0}
    };
    bool EnableOutputWav = false;
    bool EnableOutputLame = false;

    while( (c = getopt_long(argc, argv, opts, loptions, &longindex)) >= 0) {
		switch(c) {
            case 0:
                {
                    std::string arg = loptions[longindex].name;
                    if (arg == "output-wav") {
                        EnableOutputWav = true;
                    } else if (arg == "output-lame") {
                        EnableOutputLame = true;
                    } else {
                        LogError("Unknown option %s", arg.c_str());
                        exit(EXIT_FAILURE);
                    }
                }
                break;
			case 'h':
				print_help(stdout, argv[0]);
				exit(EXIT_SUCCESS);
				break;
			case 'd':
				verbose++;
				break;
            case 'v':
                printf("Version: %s\n", PACKAGE_VERSION);
                exit(EXIT_SUCCESS);
                break;
			default:
				LogError("Unknown Option");
				exit(EXIT_FAILURE);
				break;
		}
	}

	GLibLogger GLLogger;
    GstLogger GLogger;
    GLogger.SetEnabled(true);

	gst_init(&argc, &argv);

    //FIXME: Find Source index from pulseaudio based on application name.
    //Then attach to that
    PulseSource Source = PulseSource();
    DBUSMedia Watcher;
    OutputManager Output;
    bool HaveOutputs = false;

    if (EnableOutputLame) {
        HaveOutputs = true;
        std::shared_ptr<IOutputPipeline> tmp = std::make_shared<OutputLame>();
        Output.PipelineAdd(tmp);
    }

    //Default to wave file if it doesn't have any other outputs
    if (EnableOutputWav || HaveOutputs == false) {
        if (HaveOutputs) {
            LogInfo("No outputs specified defaulting to wav file");
        }
        std::shared_ptr<IOutputPipeline> tmp = std::make_shared<OutputWav>();
        Output.PipelineAdd(tmp);
    }

    //FIXME: Glue File name extraction to OutputManager

    Source.SetFunction(std::bind(&OutputManager::PushBuffer, &Output, std::placeholders::_1, std::placeholders::_2));
    Source.Start();

    Watcher.Run(); //FIXME: This currently never returns

	gst_deinit();
	return 0;
}

void Logger(int Type, const char *fmt, ...) {
    static Mutex mutex;
    ScopedLock lock = ScopedLock(&mutex);
    va_list ap;
    va_start(ap, fmt);
    int res = vfprintf(stdout, fmt, ap);
    if (res < 0)
        abort();
    fprintf(stdout, "\n");
    va_end(ap);
}
