
#include "Agent.hpp"

Agent::Agent(void (*entryPoint)(void *), const char *taskName, uint32_t stackDepth, UBaseType_t taskPriority)
        : entry_point(entryPoint), task_name(taskName), stack_depth(stackDepth), task_priority(taskPriority)
{

}
