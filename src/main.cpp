
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
    fprintf(fp, "    --output-wav               Output to wav file\n");
    fprintf(fp, "    --output-lame              Output to mp3 file using lame encoder\n");
    fprintf(fp, "    --output-lame-rtsp <url>   Output rtsp server in record mode using lame encoder\n");
    fprintf(fp, "    --output-scopebasic        Output to a basic scope (graphics)\n");
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
        {"output-lame-rtsp", 1, 0, 0},
        {"output-scopebasic", 0, 0, 0},
        {0, 0, 0, 0}
    };
    bool EnableOutputWav = false;
    bool EnableOutputLame = false;
    std::string LameRtspUri = "";
    bool EnableScopeBasic = false;

    while( (c = getopt_long(argc, argv, opts, loptions, &longindex)) >= 0) {
		switch(c) {
            case 0:
                {
                    std::string arg = loptions[longindex].name;
                    if (arg == "output-wav") {
                        EnableOutputWav = true;
                    } else if (arg == "output-lame") {
                        EnableOutputLame = true;
                    } else if (arg == "output-lame-rtsp") {
                        LameRtspUri = optarg;
                    } else if (arg == "output-scopebasic") {
                        EnableScopeBasic = true;
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

    std::unique_ptr<GLibLogger> GLLogger;
    std::unique_ptr<GstLogger> GLogger;
    if (verbose) {
        GLLogger.reset(new GLibLogger());
        GLogger.reset(new GstLogger());
        GLogger->SetEnabled(true);
    }

	gst_init(&argc, &argv);

    //FIXME: Find Source index from pulseaudio based on application name.
    //Then attach to that
    PulseSource Source = PulseSource();
    OutputManager Output;
    DBUSMedia Watcher = DBUSMedia(&Output);
    bool HaveOutputs = false;

    if (EnableOutputLame) {
        HaveOutputs = true;
        std::shared_ptr<IOutputPipeline> tmp = std::make_shared<OutputLame>();
        Output.PipelineAdd(tmp);
        Logger(LOG_DEBUG, "Adding Output Lame");
    }

    if (LameRtspUri.empty() == false) {
        HaveOutputs = true;
        std::shared_ptr<IOutputPipeline> tmp = std::make_shared<OutputLameRtsp>(LameRtspUri);
        Output.PipelineAdd(tmp);
        Logger(LOG_DEBUG, "Adding Output Lame Rtsp");
    }

    if (EnableScopeBasic) {
        HaveOutputs = true;
        std::shared_ptr<IOutputPipeline> tmp = std::make_shared<OutputScopeBasic>();
        Output.PipelineAdd(tmp);
        Logger(LOG_DEBUG, "Adding Output Scope Basic");
    }

    //Default to wave file if it doesn't have any other outputs
    if (EnableOutputWav || HaveOutputs == false) {
        if (HaveOutputs) {
            LogInfo("No outputs specified defaulting to wav file");
        }
        std::shared_ptr<IOutputPipeline> tmp = std::make_shared<OutputWav>();
        Output.PipelineAdd(tmp);
        Logger(LOG_DEBUG, "Adding Output wav");
    }

    Logger(LOG_DEBUG, "Starting Output Manager");
    Output.Start();

    Logger(LOG_DEBUG, "Starting Source");
    Source.SetFunction(std::bind(&OutputManager::PushBuffer, &Output, std::placeholders::_1, std::placeholders::_2));
    Source.Start();

    Logger(LOG_DEBUG, "Starting BUSWatch");
    Watcher.Run(); //FIXME: This currently never returns

    if (verbose) {
        GLLogger.reset();
        GLogger.reset();
    }

	gst_deinit();
	return 0;
}

void Logger(int Type, const char *fmt, ...) {
    static Mutex mutex;
    ScopedLock lock = ScopedLock(&mutex);

    if (Type == LOG_DEBUG && verbose == 0)
        return; //Ignore debug message unless debug is turned on

    va_list ap;
    va_start(ap, fmt);
    int res = vfprintf(stdout, fmt, ap);
    if (res < 0)
        abort();
    fprintf(stdout, "\n");
    va_end(ap);
}
