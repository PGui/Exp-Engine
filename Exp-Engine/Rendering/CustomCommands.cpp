#include "CustomCommands.h"

const cb::RenderContext::function_t Exp::BeginFrameCommand::kDispatchFunction = &Exp::BeginFrameCommand::execute;