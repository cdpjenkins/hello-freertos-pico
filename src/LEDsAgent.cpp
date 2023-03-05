
#include <cstdio>

#include "FreeRTOS.h"
#include <queue.h>

#include "LEDsAgent.hpp"

#include "Agent.hpp"

void leds_main(void *params) {
    LEDsAgent *agent = static_cast<LEDsAgent *>(params);

    agent->task_main();
}

LEDsAgent::LEDsAgent(void (*entryPoint)(void *), const char *taskName, uint32_t stackDepth,
                     UBaseType_t taskPriority)
    : Agent(entryPoint, taskName, stackDepth, taskPriority)
{
}

[[noreturn]]
void LEDsAgent::task_main() {
    pico_unicorn.init();

    while (true) {
        LEDsCommand command;
        BaseType_t rc = xQueueReceive(command_queue, (void *)&command, 1000);
        if (rc == pdTRUE) {
            int i = 0;
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    pico_unicorn.set_pixel(x, y, command.pixels[i++], command.pixels[i++], command.pixels[i++]);
                }
            }
        }
    }

}

void LEDsAgent::send(LEDsCommand *pCommand) {
    BaseType_t rc = xQueueSendToBack(command_queue, pCommand, 0);
    if (rc != pdTRUE) {
        printf("Failed to send message: %d\n", rc);
    }
}