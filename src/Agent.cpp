
#include <cstdio>

#include "FreeRTOS.h"
#include <queue.h>
#include <task.h>

#include "Agent.hpp"

#include "LEDsAgent.hpp"

Agent::Agent(const char *taskName, uint32_t stackDepth, UBaseType_t taskPriority)
        : task_name(taskName), stack_depth(stackDepth), task_priority(taskPriority)
{
    command_queue = xQueueCreate(16, sizeof(LEDsCommand));
}

void Agent::start() {
    BaseType_t rc = xTaskCreate(entry_point_static_function,
                                task_name,
                                stack_depth,
                                this,
                                task_priority,
                                &leds_task);
    if (rc != pdPASS) {
        printf("urgh failed to start LEDsAgent task: %d\n", rc);
    }
}

void Agent::entry_point_static_function(void *params) {
    static_cast<LEDsAgent *>(params)->task_main();
}