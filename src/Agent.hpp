#ifndef HELLO_FREERTOS_PICO_AGENT_HPP
#define HELLO_FREERTOS_PICO_AGENT_HPP

#include <portmacro.h>
#include "pico_unicorn.hpp"

class Agent {

protected:
public:
    Agent(void (*entryPoint)(void *), const char *taskName, uint32_t stackDepth, UBaseType_t taskPriority);

protected:
    void (*entry_point)(void *);
    const char *task_name;
    uint32_t stack_depth;
    UBaseType_t task_priority;
};

#endif //HELLO_FREERTOS_PICO_AGENT_HPP
