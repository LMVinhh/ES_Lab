#include <stdio.h>
#include <stdio.h>
#include <inttypes.h>
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "FreeRTOSConfig.h"
#include "freertos/timers.h"

#define eTimer1 10
#define eTimer2 20
#define eTimestamp 30
#define TIMER_1_PERIOD 2000   // 2 seconds
#define TIMER_2_PERIOD 3000   // 3 seconds
#define TIMESTAMP_PERIOD 10   // 3 seconds

#define TIMER_1_REPEAT_COUNT 10
#define TIMER_2_REPEAT_COUNT 5

TimerHandle_t timer1;
TimerHandle_t timer2;
TimerHandle_t timerstamp;

uint32_t counter_time = 1;

int timer1_counter = 0;
int timer2_counter = 0;
void vTimerCallback(TimerHandle_t xTimer){
    BaseType_t timerID = (BaseType_t) pvTimerGetTimerID(xTimer);
    switch (timerID){
        case eTimestamp:
            counter_time++;
            break;
        case eTimer1:
            printf("ahihi callback at timestamp = %lds\n", counter_time / pdMS_TO_TICKS(1000));
            timer1_counter++;
            
            if (timer1_counter >= TIMER_1_REPEAT_COUNT) {
                if (xTimerStop(xTimer, 0) == pdPASS)
                {
                    printf("Reached %d times, Stop timer ahihi successfully\n", timer1_counter);
                }
                else 
                {
                    printf("Stop timer ahihi failed\n");
                }
            }
            break;
        case eTimer2:
            printf("ihaha callback at timestamp = %lds\n", counter_time / pdMS_TO_TICKS(1000));
            timer2_counter++;
            if (timer2_counter >= TIMER_2_REPEAT_COUNT) {
                if (xTimerStop(xTimer, 0) == pdPASS)
                {
                    printf("Reached %d times, Stop timer ihaha successfully\n", timer2_counter);
                }
                else 
                {
                    printf("Stop timer ahihi failed\n");
                }
            }
            break;           

        default:
            break;

    }
}

void app_main(void)
{
    timerstamp = xTimerCreate("Timer1", TIMESTAMP_PERIOD / portTICK_PERIOD_MS, pdTRUE, (void *)eTimestamp, vTimerCallback);
    timer1 = xTimerCreate("Timer1", TIMER_1_PERIOD / portTICK_PERIOD_MS, pdTRUE, (void *)eTimer1, vTimerCallback);
    timer2 = xTimerCreate("Timer2", TIMER_2_PERIOD / portTICK_PERIOD_MS, pdTRUE, (void *)eTimer2, vTimerCallback);
    if( timer1 == NULL || timer2 == NULL || timerstamp == NULL) // check if the timer is created
    
        {
            printf("xTimerCreate() API failed !\n");
        }
    else // check if the timer is started
        {
            if( xTimerStart(timer1, 0) != pdPASS )
            {
                printf("The Timer started failed !\n");
            }
            else 
            {
                printf("Initialized timer 1 successfully \n");
            }
             if( xTimerStart(timer2, 0) != pdPASS )
            {
                printf("The Timer started failed !\n");
            }
            else 
            {
                printf("Initialized timer 2 successfully \n");
            }
              if( xTimerStart(timerstamp, 0) != pdPASS )
            {
                printf("The Timer started failed !\n");
            }
            else 
            {
                printf("Initialized timestamp successfully \n");
            }
        }
        
    }



