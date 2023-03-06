#ifndef HELLO_FREERTOS_PICO_CLIAGENT_HPP
#define HELLO_FREERTOS_PICO_CLIAGENT_HPP

#include "Agent.hpp"
#include "LEDsAgent.hpp"
#include "CommandInterpreterAgent.hpp"

class CLIAgent : public Agent {
public:
    CLIAgent(LEDsAgent &agent, CommandInterpreterAgent &interpreterAgent);

    [[noreturn]]
    void task_main() override;
private:
    LEDsAgent &leds_agent;
    CommandInterpreterAgent &interpreter_agent;

    static const UBaseType_t TASK_PRIORITY = tskIDLE_PRIORITY + 1UL;
};


#endif //HELLO_FREERTOS_PICO_CLIAGENT_HPP
