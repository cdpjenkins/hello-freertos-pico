/***
 * Demo program to flash an LED attached to GPIO PAD 0.
 * Uses FreeRTOS Task
 * Jon Durrant
 * 15-Aug-2022
 */


#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>


//Standard Task priority
#define TASK_PRIORITY		( tskIDLE_PRIORITY + 1UL )

//LED PAD to use
#define LED_PAD				0

//Blink Delay
#define DELAY				500

volatile int counter = 0;

[[noreturn]]
void print_counter_value_main(void *params){
	printf("print_counter_value_main task started\n");

	const uint ledPad = LED_PAD;
	gpio_init(ledPad);

	gpio_set_dir(ledPad, GPIO_OUT);

    while (true) { // Loop forever
//		gpio_put(ledPad, 1);
        printf("Counter is %d\n", counter);
		vTaskDelay(DELAY);
//		gpio_put(ledPad, 0);
//		vTaskDelay(DELAY);
	}
}

[[noreturn]]
void increment_counter_value_main(void *params){
    // Better not call printf in this thread because *I don't think it's threadsafe in FreeRTOS!*.
    while (true) { // Loop forever
        ++counter;
        vTaskDelay(DELAY / 3);
    }
}

void launch_tasks() {
    TaskHandle_t print_counter_value_task;
    xTaskCreate(print_counter_value_main,
                "print_counter_value_task",
                configMINIMAL_STACK_SIZE,
                nullptr,
                TASK_PRIORITY,
                &print_counter_value_task);

    TaskHandle_t increment_counter_value_task;
    xTaskCreate(increment_counter_value_main,
                "increment_counter_value_task",
                configMINIMAL_STACK_SIZE,
                nullptr,
                TASK_PRIORITY,
                &increment_counter_value_task);

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
