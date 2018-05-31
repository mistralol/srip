#include "main.h"

ScopedLock::ScopedLock(Mutex *m) {
	m_mutex = m;
	m_mutex->Lock();
	m_locked = true;
}

ScopedLock::~ScopedLock() {
	if (m_locked)
	{
		m_mutex->Unlock();
		m_locked = false;
	}
}

void ScopedLock::Lock()
{
	if (m_locked)
		abort();
	m_mutex->Lock();
	m_locked = true;
}

void ScopedLock::Unlock()
{
	if (m_locked == false)
		abort();
	m_mutex->Unlock();
	m_locked = false;
}


