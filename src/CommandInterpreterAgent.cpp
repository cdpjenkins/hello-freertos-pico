#include <FreeRTOS.h>
#include <message_buffer.h>

#include <cstdio>
#include <cstring>
#include "CommandInterpreterAgent.hpp"

CommandInterpreterAgent::CommandInterpreterAgent() :
        Agent("leds_task",
              configMINIMAL_STACK_SIZE,
              tskIDLE_PRIORITY + 1UL)
{
    message_buffer = xMessageBufferCreate(1024);
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
            printf("received: %s\n", receive_buffer);
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
