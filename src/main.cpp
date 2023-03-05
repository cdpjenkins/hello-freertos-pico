#include <stdexcept>
#include <cstdio>

#include "pico/stdlib.h"
#include "pico_unicorn.hpp"
using namespace pimoroni;

#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <cstring>

#include "oi.h"
#include "babi.h"
#include "clear.h"
#include "LEDsAgent.hpp"
#include "Agent.hpp"

#define TASK_PRIORITY		( tskIDLE_PRIORITY + 1UL )

#define LED_PAD				0

// not that we're blinking anything at the moment
#define BLINK_DELAY 500

volatile int counter = 0;

// TODO - ultimately this won't need to be global; we'll pass a reference to the CLI task
LEDsAgent leds_agent;

void task_stats() {
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    unsigned long ulTotalRunTime;

    uxArraySize = uxTaskGetNumberOfTasks();
    printf("Number of tasks %lu\n", uxArraySize);

    pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc( uxArraySize * sizeof(TaskStatus_t) );

    if(pxTaskStatusArray != nullptr) {
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray,
                                           uxArraySize,
                                           &ulTotalRunTime);

        for( x = 0; x < uxArraySize; x++ )
        {
            printf("Task: %lu \t cPri:%lu \t bPri:%lu \t hw:%4lu \t%s\n",
                   pxTaskStatusArray[ x ].xTaskNumber ,
                   pxTaskStatusArray[ x ].uxCurrentPriority ,
                   pxTaskStatusArray[ x ].uxBasePriority ,
                   pxTaskStatusArray[ x ].usStackHighWaterMark ,
                   pxTaskStatusArray[ x ].pcTaskName
            );
        }

        vPortFree(pxTaskStatusArray);
    } else {
        printf("Failed to allocate space for stats\n");
        throw std::runtime_error("Failed to allocate space for stats");
    }

    HeapStats_t heapStats;
    vPortGetHeapStats(&heapStats);
    printf("HEAP avl: %d, blocks %d, alloc: %d, free: %d\n",
           heapStats.xAvailableHeapSpaceInBytes,
           heapStats.xNumberOfFreeBlocks,
           heapStats.xNumberOfSuccessfulAllocations,
           heapStats.xNumberOfSuccessfulFrees
    );
}

[[noreturn]]
void stats_main([[maybe_unused]] void *params) {
    while (true) {
        task_stats();
        vTaskDelay(1000);
    }
}

[[noreturn]]
void print_counter_value_main([[maybe_unused]] void *params){
	printf("print_counter_value_main task started\n");

	const uint ledPad = LED_PAD;
	gpio_init(ledPad);

	gpio_set_dir(ledPad, GPIO_OUT);

    while (true) {
//		gpio_put(ledPad, 1);
        printf("Counter is %d\n", counter);
		vTaskDelay(BLINK_DELAY);
//		gpio_put(ledPad, 0);
//		vTaskDelay(BLINK_DELAY);
	}
}

[[noreturn]]
void increment_counter_value_main([[maybe_unused]] void *params){
    // Better not call printf in this thread because *I don't think it's threadsafe in FreeRTOS!*.
    while (true) { // Loop forever
        ++counter;
        vTaskDelay(BLINK_DELAY / 3);
    }
}

[[noreturn]]
void cli_main([[maybe_unused]] void *params) {
    while (true) {
        BaseType_t rc;
        char line[256];

        printf("$ ");

        char latest_char = '\0';
        for (char *cursor = line; latest_char != '\r'; ) {
            int aChar = getchar();
            printf("%c", aChar);
            latest_char = aChar;
            if (latest_char == '\r') {
                *cursor++ = '\0';
            } else {
                *cursor++ = latest_char;
            }
        }
        printf("\n");

        // TODO pretty sure we could try some C++ string stuff here and make this marginally less hellish...

        if (strcmp(line, "oi") == 0) {
            LEDsCommand oi_command = LEDsCommand(oi);
            leds_agent.send(&oi_command);
        } else if (strcmp(line, "babi") == 0) {
            LEDsCommand babi_command = LEDsCommand(babi);
            leds_agent.send(&babi_command);
        } else if (strcmp(line, "clear") == 0) {
            LEDsCommand clear_command = LEDsCommand(clear);
            leds_agent.send(&clear_command);
        } else if (strcmp(line, "stats") == 0) {
            task_stats();
        }
    }
}

void launch_tasks() {
    TaskHandle_t cli_task;
    xTaskCreate(cli_main,
                "cli_task",
                configMINIMAL_STACK_SIZE,
                nullptr,
                TASK_PRIORITY,
                &cli_task);

    leds_agent.start();

    vTaskStartScheduler();
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    printf("GO\n");

    //Start tasks and scheduler
    const char *rtos_name = "FreeRTOS";
    printf("Starting %s on core 0:\n", rtos_name);
    launch_tasks();

    return 0;
}
