
/**
 * @class ScopedLock
 * @brief Proved a class which locks a Mutex while it is in scope
 *
 * Used to shorten Lock / Unlock code and make it nearly impossible
 * to leave a mutex locked. eg when an exception occurs.
 *
 */

class ScopedLock {
	public:
		ScopedLock(Mutex *m);
		~ScopedLock();

		void Lock();
		void Unlock();

		private:
			Mutex *m_mutex;
			bool m_locked;
};


