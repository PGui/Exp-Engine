#pragma once

#include <mutex>
#include <condition_variable>

namespace Exp
{
	class Semaphore
	{
	private:
		std::mutex m_mutex;
		std::condition_variable m_condition;
		unsigned long m_count = 0; // Initialized as locked.

	public:
		void Signal();
		void Wait();
		bool TryWait();
	};
}
