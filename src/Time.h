
/** 
 * @class Time
 * @brief Utility / Helper Functions for time operations on structures / functions
 * Provides various utility functions for helping to deal with time date / time structures and functions
 */

class Time
{
	public:
		static void Sleep(const struct timespec *Timeout);
		static void Sleep(const struct timeval *Timeout);

		static void TimeSpecToTimeVal(const struct timespec *ts, struct timeval *tv);
		static void TimeValtoTimeSpec(const struct timeval *tv, struct timespec *ts);

		static void TimeSpecFromNanoSeconds(uint64_t, struct timespec *ts);
		static void TimeSpecFromMicroSeconds(uint64_t, struct timespec *ts);
		static void TimeSpecFromMilliSeconds(uint64_t, struct timespec *ts);
		static void TimeSpecFromSeconds(uint64_t, struct timespec *ts);
		static void TimeSpecFromMinutes(uint64_t, struct timespec *ts);
		static void TimeSpecFromHours(uint64_t, struct timespec *ts);
		static void TimeSpecFromDays(uint64_t, struct timespec *ts);

		static void TimeValFromNanoSeconds(uint64_t, struct timeval *ts);
		static void TimeValFromMicroSeconds(uint64_t, struct timeval *ts);
		static void TimeValFromMilliSeconds(uint64_t, struct timeval *ts);
		static void TimeValFromSeconds(uint64_t, struct timeval *ts);
		static void TimeValFromMinutes(uint64_t, struct timeval *ts);
		static void TimeValFromHours(uint64_t, struct timeval *ts);
		static void TimeValFromDays(uint64_t, struct timeval *ts);

		static uint64_t NanoSeconds(const struct timespec *ts);
		static uint64_t NanoSeconds(const struct timeval *ts);

		static uint64_t MicroSeconds(const struct timespec *ts);
		static uint64_t MicroSeconds(const struct timeval *ts);

		static uint64_t MilliSeconds(const struct timespec *ts);
		static uint64_t MilliSeconds(const struct timeval *ts);

		static uint64_t Seconds(const struct timespec *ts);
		static uint64_t Seconds(const struct timeval *ts);

		static uint64_t Minutes(const struct timespec *ts);
		static uint64_t Minutes(const struct timeval *ts);

		static uint64_t Hours(const struct timespec *ts);
		static uint64_t Hours(const struct timeval *ts);

		static uint64_t Days(const struct timespec *ts);
		static uint64_t Days(const struct timeval *ts);

		static void Add(const struct timespec *ts1, const struct timespec *ts2, struct timespec *res);
		static void Add(const struct timeval *tv1, const struct timeval *tv2, struct timeval *res);

		static void Sub(const struct timespec *ts1, const struct timespec *ts2, struct timespec *res);
		static void Sub(const struct timeval *tv1, const struct timeval *tv2, struct timeval *res);

		static void Divide(const struct timespec *ts, int div, struct timespec *res);
		static void Divide(const struct timeval *ts, int div, struct timeval *res);
		
		static void Multiply(const struct timespec *ts, int x, struct timespec *res);
		static void Multiply(const struct timeval *ts, int x, struct timeval *res);

		static void Diff(const struct timespec *ts1, const struct timespec *ts2, struct timespec *res);
		static void Diff(const struct timeval *tv1, const struct timeval *tv2, struct timeval *res);

		static bool IsLess(const struct timespec *ts1, const struct timespec *ts2);
		static bool IsLess(const struct timeval *tv1, const struct timeval *tv2);

		static bool IsGreater(const struct timespec *ts1, const struct timespec *ts2);
		static bool IsGreater(const struct timeval *tv1, const struct timeval *tv2);

		static void Max(struct timespec *ts);
		static void Max(struct timeval *tv);

		static void Zero(struct timespec *ts);
		static void Zero(struct timeval *tv);

		static void MonoTonic(struct timespec *ts);
		static void MonoTonic(struct timeval *tv);
		
		static void UTCNow(struct timespec *ts);
		static void UTCNow(struct timeval *tv);

		static std::string ToHuman(const struct timespec *ts);
		static std::string ToHuman(const struct timeval *tv);
};

