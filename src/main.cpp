
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <mutex>

#include <gst/gst.h>

#include <main.h>

int verbose = 0;

static void print_help(FILE *fp, char *app) {
    fprintf(fp, "Usage: %s <options>\n", app);
    fprintf(fp, "\n");
    fprintf(fp, " -h --help           Print this help\n");
    fprintf(fp, " -d --debug          Switch on debug level logging\n");
    fprintf(fp, "\n");
}

int main(int argc, char **argv) {

	const char *opts = "hd";
    int longindex = 0;
    int c = 0;
    struct option loptions[] {
        {"help", 0, 0, 'h'},
        {"debug", 0, 0, 'd'},
        {0, 0, 0, 0}
    };

    while( (c = getopt_long(argc, argv, opts, loptions, &longindex)) >= 0) {
		switch(c) {
			case 'h':
				print_help(stdout, argv[0]);
				exit(EXIT_SUCCESS);
				break;
			case 'd':
				verbose++;
				break;
			default:
				LogError("Unknown Option");
				exit(EXIT_FAILURE);
				break;
		}
	}



	gst_init(&argc, &argv);


	gst_deinit();
	return 0;
}

void Logger(int Type, const char *fmt, ...) {
    static std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);
    va_list ap;
    va_start(ap, fmt);
    int res = vfprintf(stdout, fmt, ap);
    if (res < 0)
        abort();
    fprintf(stdout, "\n");
    va_end(ap);
}
