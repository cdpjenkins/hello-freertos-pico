#include <cstdio>

#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#include "oi.h"
#include "babi.h"
#include "clear.h"
#include "LEDsAgent.hpp"
#include "CLIAgent.hpp"
#include "CommandInterpreterAgent.hpp"

void launch_tasks() {
    LEDsAgent leds_agent;
    leds_agent.start();

    CommandInterpreterAgent command_interpreter_agent(leds_agent);
    command_interpreter_agent.start();

    CLIAgent cli_agent(leds_agent, command_interpreter_agent);
    cli_agent.start();

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
