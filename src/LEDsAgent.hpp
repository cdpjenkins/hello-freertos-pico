#ifndef HELLO_FREERTOS_PICO_LEDSAGENT_HPP
#define HELLO_FREERTOS_PICO_LEDSAGENT_HPP

#include "pico_unicorn.hpp"

#include "Agent.hpp"

struct LEDsCommand {
    explicit LEDsCommand() : pixels(nullptr) {}
    explicit LEDsCommand(const unsigned char *string) : pixels(string) {}

    const unsigned char* pixels;
};

class LEDsAgent : Agent {
public:
    LEDsAgent();

    void start();
    void send(LEDsCommand *pCommand);

    // TODO - maybe we could turn the function that calls this into a static function...
    [[noreturn]]
    void task_main();

    static const int WIDTH = 16;
    static const int HEIGHT = 7;

private:
    static const UBaseType_t TASK_PRIORITY = tskIDLE_PRIORITY + 1UL;

    TaskHandle_t leds_task;
    QueueHandle_t led_command_queue;

    pimoroni::PicoUnicorn pico_unicorn;
};

#endif //HELLO_FREERTOS_PICO_LEDSAGENT_HPP
