
#include <stdio.h>
#include <inttypes.h>
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "FreeRTOSConfig.h"

#include "mode.h"
#include "freertos/semphr.h"

// Global variable for idle task tick of each core
volatile uint64_t idleTicksCore0 = 0UL;
volatile uint64_t idleTicksCore1 = 0UL;

// Time period over which to calculate CPU utilization.
#define MONITOR_PERIOD_TASK1 (2000 / portTICK_PERIOD_MS) // Translated to ticks
#define MONITOR_PERIOD_TASK2 (2000 / portTICK_PERIOD_MS) // Translated to ticks
TaskHandle_t task1Handle, task2Handle,IDLEhandle;

void vTask1(void *pvParameter) 
{
    while(1) 
    {
        printf("vTask1 is running on core: %d\n", xPortGetCoreID());
        #ifdef SCHEDULING_MODE_CO_OP
        taskYIELD();  // to give other task opportunity to enter running state
        #endif
        //for (int i = 0; i < 2000000; i++);
        xTaskNotifyGive(task2Handle);
        ulTaskNotifyTake(pdTRUE, MONITOR_PERIOD_TASK1);

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
        //for (int i = 0; i < 1000000; i++);
        ulTaskNotifyTake(pdTRUE, MONITOR_PERIOD_TASK2);
        vTaskDelay(MONITOR_PERIOD_TASK2);  
    }
}
// void Task1(void* pvParameters) {
//     // Wait for notification from Task 2
//     ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

//     // Task 1 code here
// }

// void Task2(void* pvParameters) {
//     // Task 2 code here

//     // Notify Task 1
//     vTaskNotifyGive(Task1Handle);

//     // Task 2 continues its execution
// }
void vIDLE(void *pvParameter) 
{
    while(1) 
    {
        printf("This is IDLE task in  %d\n", xPortGetCoreID());
        #ifdef SCHEDULING_MODE_CO_OP
        taskYIELD();  // to give other task opportunity to enter running state
        #endif
        xTaskNotifyGive(task1Handle);

        //for (int i = 0; i < 1000000; i++);
        idleTicksCore0++;
         idleTicksCore1++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);  
    }
}
void vApplicationIdleHook()
{
    if (xPortGetCoreID() == 0) {
        idleTicksCore0++;
    } else {
        idleTicksCore1++;
    }
}

static void vMonitorTask1(void* arg) {
    while (1) {
        // Sleep for the monitor period
        vTaskDelay(MONITOR_PERIOD_TASK1);
        printf("this idleTicksCore0 have value :%lld\n", idleTicksCore0);
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
        printf("this idleTicksCore1 have value :%lld\n", idleTicksCore1);

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
    #if defined(SCHEDULING_MODE_TIME_SLICING)
    xTaskCreatePinnedToCore(vTask1, "Task1", 2048, &task1Handle, 1, NULL);
    xTaskCreatePinnedToCore(vTask2, "Task2", 2048, &task2Handle, 1, NULL);
    #elif  defined(SCHEDULING_MODE_CO_OP)
    xTaskCreatePinnedToCore(vTask1, "Task1", 2048, NULL, 1, &task1Handle, 0);  // Higher priority
    xTaskCreatePinnedToCore(vTask2, "Task2", 2048, NULL, 2, &task2Handle, 1);  // Lower priority
    xTaskCreatePinnedToCore(vIDLE, "IDLE", 2048, NULL, tskIDLE_PRIORITY, &IDLEhandle, 1);  // Lower priority
    xTaskCreatePinnedToCore(vIDLE, "IDLE", 2048, NULL, tskIDLE_PRIORITY, &IDLEhandle, 0);  // Lower priority


    #endif

    // Extra exercise
    xTaskCreate(vMonitorTask1, "Monitor Core0 usage", 4096, &task1Handle, 0, NULL);
    xTaskCreate(vMonitorTask2, "Monitor Core1 usage", 4096, &task2Handle, 1, NULL);
}