#ifndef HELLO_FREERTOS_PICO_AGENT_HPP
#define HELLO_FREERTOS_PICO_AGENT_HPP

#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#include <queue.h>
#include "pico_unicorn.hpp"

class Agent {
public:
    Agent(const char *taskName, uint32_t stackDepth, UBaseType_t taskPriority);
    void start();

    static void entry_point_static_function(void *params);
protected:
    virtual void task_main() = 0;
private:
    TaskHandle_t leds_task;
    const char *task_name;
    uint32_t stack_depth;
    UBaseType_t task_priority;
};

#endif //HELLO_FREERTOS_PICO_AGENT_HPP
