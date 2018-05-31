#include "main.h"

Mutex::Mutex() {
	pthread_mutexattr_t mattr;
	m_clocktype = CLOCK_REALTIME;

	if (pthread_mutexattr_init(&mattr) != 0)
		abort();

	if (pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE) != 0)
		abort();

	if (pthread_mutex_init(&m_mutex, &mattr) != 0)
		abort();

	if (pthread_mutexattr_destroy(&mattr) != 0)
		abort();

	pthread_condattr_t cattr;

	if (pthread_condattr_init(&cattr) != 0)
		abort();

	//Try to use monotonic clock so date time change do not effect us if we are sleeping on a condition
	if (pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC) == 0)
		m_clocktype = CLOCK_MONOTONIC;


	if (pthread_cond_init(&m_cond, &cattr) != 0)
		abort();

	if (pthread_condattr_destroy(&cattr) != 0)
		abort();

	m_locked = false;
	m_depth = 0;
}

/**
 * ~Mutex
 *
 * Destructor for Mutex. This should never fail. If it does it will call abort.
 *
 * If DEBUG is defined it will check if the Mutex is still locked.
 * If it is it will call abort as it indicates a software bug.
 */
Mutex::~Mutex() {
#ifdef DEBUG
	if (m_locked)
		abort(); //Attempts to free mutex that is locked
#endif

	if (pthread_cond_destroy(&m_cond) != 0)
		abort();

	if (pthread_mutex_destroy(&m_mutex) != 0)
		abort();
}

/**
 * Lock
 *
 * Lock the Mutex. This function should never fail. Though it may hang forever
 * if there is a bug in the application using the Mutex class
 */
void Mutex::Lock() {
	if (pthread_mutex_lock(&m_mutex) != 0)
		abort(); //Could not lock mutex
	m_locked = true;
	m_owner = pthread_self();
	m_depth++;
}

/**
 * TimedLock
 * @param[in] Timeout The length of time to wait for a timeout
 * @return Will return 0 if the lock is successful. Otherwise -errno
 *
 * Will attempt to take a lock of the mutex.
 * If the mutex is already locked by another thread then the function will sleep for the Timeout period.
 * The error returned should be -ETIMEOUT if the Timeout expires before the lock is aquired.
 * Any other error should be treated as asoftware bug
 */
int Mutex::TimedLock(const struct timespec *Timeout)
{
	struct timespec now, then;

	if (clock_gettime(m_clocktype, &now) < 0)
		abort();

	Time::Add(Timeout, &now, &then);

	int ret = pthread_mutex_timedlock(&m_mutex, Timeout);
	if (ret == 0)
	{
		m_locked = true;
		m_owner = pthread_self();
		m_depth++;
		return 0;
	}
	return -errno;
}

/**
 * TryLock
 * @return Will return 0 if the lock is successful. Otherwise -errno
 *
 * This is an alias of timedlock with the timeout set to zero
 */
int Mutex::TryLock()
{
	struct timespec ts = {0, 0};
	return TimedLock(&ts);
}

/**
 * Unlock
 *
 * Unlocks a locked mutex
 * This function should never fail. If it does abort will be called.
 * Abort could be called if the caller attempt to unlock a mutex that is not locked.
 * Which is a software bug on the caller side.
 *
 */
void Mutex::Unlock() {
	m_locked = false;
	m_depth--;
	if (pthread_mutex_unlock(&m_mutex) != 0)
		abort(); //Could not unlock mutex
}

/**
 *
 * Returns true if the current thread is the current owner of the lock
 * It is useful for debug code to check that a lock is held when specific functions are called
 */
bool Mutex::IsOwner()
{
	if (m_locked == false)
		return false;
	if (pthread_equal(m_owner, pthread_self()))
		return true;
	return false;
}

/**
 * Wait
 *
 * Wait until another thread calls WakeUp or WakeUpAll
 *
 * The thread must already hold the lock before this function is called.
 * If DEBUG is defined at compile time this will cause abort to be called if the Mutex is not locked.
 */
void Mutex::Wait() {
#ifdef DEBUG
	if (m_locked == false)
		abort();
	if (m_depth > 1)
		abort(); //Does not work well with recursive mutex
#endif

	m_locked = false;
	int ret = pthread_cond_wait(&m_cond, &m_mutex);
	if (ret != 0)
		abort(); //pthread_cond_wait failed
	m_locked = true;
	m_owner = pthread_self();
}

/**
 * Wait
 * @param[in] Timeout The length of time to wait until we return
 * @return Return 0 on success. Or -errno
 *
 * Wait until another thread calls WakeUp or WakeUpAll. Or the Timeout expires. If the Timeout is reached -ETIMEOUT will be returned.
 * 
 * The thread must already hold the lock before this function is called.
 * If DEBUG is defined at compile time this will cause abort to be called if the Mutex is not locked.
 *
 * Note: It is possbile for this function to return without WakeUp or WakeUpAll being returned.
 *
 */
int Mutex::Wait(const struct timespec *Timeout) {
#ifdef DEBUG
	if (m_locked == false)
		abort();
	if (m_depth > 1)
		abort(); //Does not work well with recursive mutex
#endif
	struct timespec now, then;

	if (clock_gettime(m_clocktype, &now) < 0)
		abort();

	Time::Add(Timeout, &now, &then);

	m_locked = false;
	int ret = pthread_cond_timedwait(&m_cond, &m_mutex, &then);
	m_locked = true;
	m_owner = pthread_self();
	if (ret != 0) {
		switch(ret) {
			case ETIMEDOUT:
				return -ETIMEDOUT;
				break;
			case EBUSY:
			case EINTR:
				return 0;
			default:
				abort(); //pthread_cond_timedwait failed
				break;
		}
	}
	return ret; /* Unreachable! */
}

/**
 * WakeUp
 * 
 * WakeUp a single thread that is sleeping in WakeUp
 *
 * It is required to have the lock held when calling this function
 */
void Mutex::WakeUp() {
#ifdef DEBUG
	if (IsOwner() == false)
		abort();
#endif
	int ret = pthread_cond_signal(&m_cond);
	if (ret != 0)
		abort(); //pthread_cond_signal failed
}

/**
 * WakeUp
 * 
 * WakeUp a all threads that is sleeping in WakeUp
 *
 * It is required to have the lock held when calling this function
 */
void Mutex::WakeUpAll() {
#ifdef DEBUG
	if (IsOwner() == false)
		abort();
#endif
	int ret = pthread_cond_broadcast(&m_cond);
	if (ret != 0)
		abort(); //pthread_cond_signal failed
}

