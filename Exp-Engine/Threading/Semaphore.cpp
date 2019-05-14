#include "Semaphore.h"

namespace Exp
{
	void Semaphore::Signal() {
		std::lock_guard<std::mutex> lock(m_mutex);
		++m_count;
		m_condition.notify_one();
	}

	void Semaphore::Wait() {
		std::unique_lock<std::mutex> lock(m_mutex);
		while (!m_count) // Handle spurious wake-ups.
		{
			m_condition.wait(lock);
		}
		--m_count;
	}

	bool Semaphore::TryWait() {
		std::lock_guard<decltype(m_mutex)> lock(m_mutex);
		if (m_count) 
		{
			--m_count;
			return true;
		}
		return false;
	}
}