
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
// Variables to store idle task statistics
static TickType_t idleTimeCore0 = 0;
static TickType_t idleTimeCore1 = 0;

// Time period over which to calculate CPU utilization.
#define MONITOR_PERIOD_TASK1 (6000 / portTICK_PERIOD_MS) // Translated to ticks
#define MONITOR_PERIOD_TASK2 (2000 / portTICK_PERIOD_MS) // Translated to ticks
#define MONITOR_PERIOD_TASK_MONITOR (2000 / portTICK_PERIOD_MS) // Translated to ticks

TaskHandle_t task1Handle, task2Handle,IDLEhandle;
 #if defined(SCHEDULING_MODE_TIME_SLICING)

void vTask1(void *pvParameter) 
{
            

    while(1) 
    {
        printf("========>vTask1 is running on core: %d\n", xPortGetCoreID());
        

        vTaskDelay(MONITOR_PERIOD_TASK1); 
    }
}



void vTask2(void *pvParameter) 
{
    int count = 0;
    while(1) 
    {
        printf("vTask2 is running on core: %d and %d\n", xPortGetCoreID(),++count);
       
        //for (int i = 0; i < 1000000; i++);
        //vTaskDelay(MONITOR_PERIOD_TASK2);  
        
    }
}

void vIDLE(void *pvParameter) 
{
    int count = 0;

    while(1) 
    {
        printf("IDLE is running on core: %d and %d\n", xPortGetCoreID(),++count);
    }
}

void vApplicationIdleHook(void) {
  // Get the current tick count
  TickType_t currentTickCount = xTaskGetTickCount();

  // Determine the core on which the idle task is running
  BaseType_t coreId = xPortGetCoreID();

  // Update the idle time based on the core
  if (coreId == 0) {
    idleTimeCore0 = currentTickCount;
  } else if (coreId == 1) {
    idleTimeCore1 = currentTickCount;
  }
}

static void vMonitorTask(void* arg) {
     TickType_t totalIdleTime = idleTimeCore0 + idleTimeCore1;
    TickType_t totalRunTime = xTaskGetTickCount();
    float cpuUtilization = (float)(totalRunTime - totalIdleTime) / totalRunTime * 100.0;


    // Print CPU utilization

    printf("================================>Total CPU Utilization: %.2f%%\n", cpuUtilization);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
}



void app_main(void) 
{

    vApplicationIdleHook();
    xTaskCreate(vTask1, "Task1", 2048, NULL, 1, &task1Handle);
    xTaskCreate(vTask2, "Task2", 2048, NULL, tskIDLE_PRIORITY, &task2Handle);
    xTaskCreate(vIDLE, "IDLE", 2048, NULL, tskIDLE_PRIORITY, &IDLEhandle);  // Lower priority

    // Extra exercise
    xTaskCreate(vMonitorTask, "Monitor Core0 usage", 4096, NULL, tskIDLE_PRIORITY, &IDLEhandle);
}
    #endif


#if  defined(SCHEDULING_MODE_CO_OP)
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

SemaphoreHandle_t semaphore;

TaskHandle_t task1Handle, task2Handle, task3Handle, IDLEhandle;

void vTask1(void *pvParameter) 
{
    int count = 0;

    // Wait for the semaphore to be released
    xSemaphoreTake(semaphore, portMAX_DELAY);

    while(1) 
    {
        printf("vTask1 is running on core: %d and called %d time\n", xPortGetCoreID(), ++count);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void vTask2(void *pvParameter) 
{
    int count = 0;

    // Wait for the semaphore to be released
    xSemaphoreTake(semaphore, portMAX_DELAY);

    while(1) 
    {
        printf("vTask2 is running on core: %d and called %d time\n", xPortGetCoreID(), ++count);
                vTaskDelay(2000/portTICK_PERIOD_MS);

    }
}

// void vTask3(void *pvParameter) 
// {
//     // Wait for the semaphore to be released
//     xSemaphoreTake(semaphore, portMAX_DELAY);

//     printf("vTask3 is running on core: %d\n", xPortGetCoreID());
//     // Task 3 implementation
// }

void vIDLE(void *pvParameter) 
{
    int count = 0;

    while(1) 
    {
        printf("IDLE is running on core: %d and %d\n", xPortGetCoreID(), ++count);

        if (count == 1) {
            // Release the semaphore to unblock Task 1, Task 2, and Task 3
            xSemaphoreGive(semaphore);
        }

        // Perform cooperative scheduling
        // Perform cooperative scheduling
        if(count == 1400){
            printf("IDLE Yield!!!!!!!!!\n");

        taskYIELD();
        count = 0;
        }
    }
}

void app_main(void) 
{
    // Create a binary semaphore
    semaphore = xSemaphoreCreateBinary();

    // Create Task 1, Task 2, and Task 3
    xTaskCreate(vTask1, "Task1", 2048, NULL, 2, &task1Handle);
    xTaskCreate(vTask2, "Task2", 2048, NULL, 1, &task2Handle);

    // Create the IDLE task
    xTaskCreate(vIDLE, "IDLE", 2048, NULL, tskIDLE_PRIORITY, &IDLEhandle);
    
    // Start the scheduler
}
    #endif
