/*
 * Created by Nhom 6
*/

#include <stdio.h>
#include <inttypes.h>
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_flash.h"
#include "global.h"
#include "freertos/semphr.h"

//This define will hook with loop while in main_app as an IDLE task
#define configUSE_TICKLESS_IDLE 1

// Global variable for idle task tick of each core
volatile uint64_t idleTicksCore0 = 0;
volatile uint64_t idleTicksCore1 = 0;
// Time period over which to calculate CPU utilization.
#define MONITOR_PERIOD_TASK1 (100 / portTICK_PERIOD_MS) // Translated to ticks
#define MONITOR_PERIOD_TASK2 (150 / portTICK_PERIOD_MS) // Translated to ticks

#ifdef SCHEDULING_MODE_TIME_SLICING
	#define configUSE_TIME_SLICING 1

#endif
#ifdef SCHEDULING_MODE_WITHOUT_TIME_SLICING
	#define configUSE_TIME_SLICING 0
#endif
void vTask1(void *pvParameter) 
{
    while(1) 
    {
        printf("vTask1 is running on core: %d\n", xPortGetCoreID());
        #ifdef SCHEDULING_MODE_CO_OP
        taskYIELD();  // to give other task opportunity to enter running state
        #endif
        
        for (int i = 0; i < 2000000; i++);

        vTaskDelay(MONITOR_PERIOD_TASK1); 
    }
}

void vTask2(void *pvParameter) 
{
    while(1) 
    {
        printf("vTask2 is running on core: %d\n", xPortGetCoreID());
        #ifdef SCHEDULING_MODE_CO_OP
        taskYIELD();  // to give other task opportunity to enter running state
        #endif

        for (int i = 0; i < 1000000; i++);

        vTaskDelay(MONITOR_PERIOD_TASK2);  
    }
}

// void vApplicationIdleHook()
// {
//     if (xPortGetCoreID() == 0) {
//         idleTicksCore0++;
//     } else {
//         idleTicksCore1++;
//     }
// }

static void vMonitorTask1(void* arg) {
    while (1) {
        // Sleep for the monitor period
        vTaskDelay(MONITOR_PERIOD_TASK1);
        printf("%lld\n", idleTicksCore0);
        // Calculate the CPU utilization
        float cpu0Utilization = 100.0 - (idleTicksCore0 * 100.0 / MONITOR_PERIOD_TASK1);

        // Log the CPU utilization
        printf("CPU0 Utilization: %.2f%%\n", cpu0Utilization);

        // Update our last tick counts for the next cycle
        idleTicksCore0 = 0;
    }
}

static void vMonitorTask2(void* arg) {
    while (1) {
        // Sleep for the monitor period
        vTaskDelay(MONITOR_PERIOD_TASK2);
        printf("%lld\n", idleTicksCore1);

        // Calculate the CPU utilization
        float cpu1Utilization = 100.0 - (idleTicksCore1 * 100.0 / MONITOR_PERIOD_TASK2);

        // Log the CPU utilization
        printf("CPU1 Utilization: %.2f%%\n", cpu1Utilization);

        // Update our last tick counts for the next cycle
        idleTicksCore1 = 0;
    }
}

void app_main(void) 
{
    #if defined(SCHEDULING_MODE_TIME_SLICING) || defined(SCHEDULING_MODE_WITHOUT_TIME_SLICING)
    xTaskCreatePinnedToCore(vTask1, "Task1", 2048, NULL, 1, NULL,0);
    xTaskCreatePinnedToCore(vTask2, "Task2", 2048, NULL, tskIDLE_PRIORITY, NULL,0);

    #elif defined(SCHEDULING_MODE_PREEMPTIVE) || defined(SCHEDULING_MODE_CO_OP)
    xTaskCreatePinnedToCore(vTask1, "Task1", 2048, NULL, 2, NULL, 0);  // Higher priority
    xTaskCreatePinnedToCore(vTask2, "Task2", 2048, NULL, 1, NULL, 1);  // Lower priority
    #endif

    // Extra exercise
    xTaskCreate(vMonitorTask1, "Monitor Core0 usage", 4096, NULL, 2, NULL);
    xTaskCreate(vMonitorTask2, "Monitor Core1 usage", 4096, NULL, 2, NULL);
    while(true){
        printf("This is IDLE task\n");   
            vTaskDelay(50/portTICK_PERIOD_MS);
 }
 }