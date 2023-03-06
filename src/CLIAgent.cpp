#include "FreeRTOS.h"

#include <cstdio>

#include "CLIAgent.hpp"
#include "LEDsAgent.hpp"

CLIAgent::CLIAgent(LEDsAgent &agent, CommandInterpreterAgent &interpreterAgent) :
    Agent("cli_task",
          configMINIMAL_STACK_SIZE * 2,
          TASK_PRIORITY),
    leds_agent(agent),
    interpreter_agent(interpreterAgent)
{

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

        interpreter_agent.send_command(line);
    }
}
