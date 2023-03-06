
#include <cstdio>

#include "FreeRTOS.h"
#include <queue.h>

#include "LEDsAgent.hpp"

#include "Agent.hpp"

LEDsAgent::LEDsAgent() :
        Agent("leds_task",
              configMINIMAL_STACK_SIZE,
              tskIDLE_PRIORITY + 1UL)
{
    command_queue = xQueueCreate(16, sizeof(LEDsCommand));
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
                    unsigned char r = command.pixels[i++];
                    unsigned char g = command.pixels[i++];
                    unsigned char b = command.pixels[i++];
                    pico_unicorn.set_pixel(x, y, r, g, b);
                }
            }
        }
    }
}

void LEDsAgent::send(LEDsCommand *pCommand) {
    BaseType_t rc = xQueueSendToBack(command_queue, pCommand, 0);
    if (rc != pdTRUE) {
        printf("Failed to send message: %ld\n", rc);
    }
}

