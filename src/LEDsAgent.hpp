#ifndef HELLO_FREERTOS_PICO_LEDSAGENT_HPP
#define HELLO_FREERTOS_PICO_LEDSAGENT_HPP

#include <queue.h>
#include <task.h>
#include <portmacro.h>
#include "pico_unicorn.hpp"

#include "Agent.hpp"

struct LEDsCommand {
    explicit LEDsCommand() : pixels(nullptr) {}
    explicit LEDsCommand(const unsigned char *string) : pixels(string) {}

    const unsigned char* pixels;
};

class LEDsAgent : public Agent {
public:
    LEDsAgent(void (*entryPoint)(void *), const char *taskName, uint32_t stackDepth,
              UBaseType_t taskPriority);

    static const int WIDTH = 16;
    static const int HEIGHT = 7;

    void send(LEDsCommand *pCommand);

    [[noreturn]]
    void task_main();

private:
    static const UBaseType_t TASK_PRIORITY = tskIDLE_PRIORITY + 1UL;

    pimoroni::PicoUnicorn pico_unicorn;

};

#endif //HELLO_FREERTOS_PICO_LEDSAGENT_HPP
