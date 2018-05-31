
class Mutex {

	public:
		Mutex();
		~Mutex();

		void Lock();
		int TimedLock(const struct timespec *Timeout);
		int TryLock();
		void Unlock();

		bool IsOwner();

		void Wait();
		int Wait(const struct timespec *Timeout);
		void WakeUp();
		void WakeUpAll();


	protected:
		pthread_mutex_t m_mutex;
		pthread_cond_t m_cond;
		clockid_t m_clocktype;
		bool m_locked;
		pthread_t m_owner;
		int m_depth;
};


