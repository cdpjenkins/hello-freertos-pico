#include "FreeRTOS.h"
#include <queue.h>
#include <cstdio>

#include "LEDsAgent.hpp"

void leds_main(void *params){
    LEDsAgent *agent = static_cast<LEDsAgent *>(params);

    agent->task_main();
}

LEDsAgent::LEDsAgent() {
    led_command_queue = xQueueCreate( 16, sizeof(LEDsCommand));
}

void LEDsAgent::start() {
    BaseType_t rc = xTaskCreate(leds_main,
                                "leds_task",
                                configMINIMAL_STACK_SIZE,
                                this,
                                TASK_PRIORITY,
                                &leds_task);
    if (rc != pdPASS) {
        printf("urgh failed to start LEDsAgent task: %d\n", rc);
    }
}

[[noreturn]]
void LEDsAgent::task_main() {
    pico_unicorn.init();

    while (true) {
        LEDsCommand command;
        BaseType_t rc = xQueueReceive(led_command_queue, (void *)&command, 1000);
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
    BaseType_t rc = xQueueSendToBack(led_command_queue, pCommand, 0);
    if (rc != pdTRUE) {
        printf("Failed to send message: %d\n", rc);
    }
}
