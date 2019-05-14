#pragma once
#include <ftl/thread_abstraction.h>
#include <ftl/task_scheduler.h>
#include <ftl/atomic_counter.h>
#include <ftl/task.h>
#include "../Core/Engine.h"

namespace Exp
{
	FTL_THREAD_FUNC_DECL RenderThreadStart(void *arg);
}

