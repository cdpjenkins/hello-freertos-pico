#include "FreeRTOS.h"
#include "task.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "CLIAgent.hpp"
#include "LEDsAgent.hpp"
#include "oi.h"
#include "babi.h"
#include "clear.h"

CLIAgent::CLIAgent(LEDsAgent &agent) :
    Agent("cli_task",
          configMINIMAL_STACK_SIZE * 2,
          TASK_PRIORITY),
    leds_agent(agent)
{

}


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


void CLIAgent::task_main() {
    while (true) {
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
            auto oi_command = LEDsCommand(oi);
            leds_agent.send(&oi_command);
        } else if (strcmp(line, "babi") == 0) {
            auto babi_command = LEDsCommand(babi);
            leds_agent.send(&babi_command);
        } else if (strcmp(line, "clear") == 0) {
            auto clear_command = LEDsCommand(clear);
            leds_agent.send(&clear_command);
        } else if (strcmp(line, "stats") == 0) {
            task_stats();
        }
    }

}