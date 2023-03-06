#include <FreeRTOS.h>
#include <message_buffer.h>

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include "CommandInterpreterAgent.hpp"
#include "oi.h"
#include "babi.h"
#include "clear.h"

CommandInterpreterAgent::CommandInterpreterAgent(LEDsAgent &agent) :
        Agent("leds_task",
              configMINIMAL_STACK_SIZE,
              tskIDLE_PRIORITY + 1UL),
              leds_agent(agent)
{
    message_buffer = xMessageBufferCreate(1024);
}

static void task_stats() {
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


void CommandInterpreterAgent::task_main() {

    char receive_buffer[1024];

    while (true) {
        size_t receive_length = xMessageBufferReceive(
                message_buffer,
                receive_buffer,
                1024,
                1000
        );
        if (receive_length > 0) {
            if (strcmp(receive_buffer, "oi") == 0) {
                auto oi_command = LEDsCommand(oi);
                leds_agent.send(&oi_command);
            } else if (strcmp(receive_buffer, "babi") == 0) {
                auto babi_command = LEDsCommand(babi);
                leds_agent.send(&babi_command);
            } else if (strcmp(receive_buffer, "clear") == 0) {
                auto clear_command = LEDsCommand(clear);
                leds_agent.send(&clear_command);
            } else if (strcmp(receive_buffer, "stats") == 0) {
                task_stats();
            }
        } else {
            // I guess 0 means nothing was sent
        }
    }
}

void CommandInterpreterAgent::send_command(char *command_string) {
    size_t command_length = strlen(command_string) + 1;

    size_t  res = xMessageBufferSend(
            message_buffer,
            command_string,
            command_length,
            0);

    if (res != command_length){
        printf("ERROR: failed to write whole message to buffer\n");
    }
}
