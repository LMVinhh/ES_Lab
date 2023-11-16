#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

xQueueHandle qHandler;
typedef struct
{
    int eDataID; // ID task
    int lDataValue; // Gia trị mà task đó nhận
} Data_t;

// Hàm nhận task theo ID và data 
// Id của mỗi task sẽ setup theo ngẫu nhiên từ 1 - 3 do chỉ có 3 task
void reception(void *pvParameter)
{
    Data_t requestPtr;
    uint32_t data = 0;
    srand(time(0));
    while (true)
    {
        requestPtr.eDataID = (rand() % 3) + 1;
        requestPtr.lDataValue = data;
        data++;
        xQueueSendToBack(qHandler, &requestPtr, portMAX_DELAY);
        printf("Receiving task with ID : %d, Data: %d into the last position of the Queue.\n", requestPtr.eDataID, requestPtr.lDataValue);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void func_3(Data_t request)
{
    if (qHandler != NULL)
    {
        if (request.eDataID > 2)
            printf("func_3 Ignore ID: %d, Data: %d.\n", request.eDataID, request.lDataValue);
    }
    else
        printf("func_3 is NULL");

    return;
}

void func_2(Data_t request)
{

    if (qHandler != NULL)
    {
        if (request.eDataID == 2)
        {
            printf("func_1 ID: %d, Data: %d.\n", request.eDataID, request.lDataValue);
        }
        else
        {
            printf("error func_2 !!!\n");
            func_3(request);
        }
    }
    else
        printf("Queue is empty.\n");

    return;
}
// Chạy tuần tự từng function, nếu id không khớp sẽ báo error sẽ gọi function 2 và tương tự, đến khi nào đến function 3 
// khi đến function mà không thỏa id sẽ ignore data đó
void func_1(void *pvParameter)
{

    if (qHandler != NULL)
    {
        Data_t request;
        while (true)
        {
            if (xQueueReceive(qHandler, &request, pdMS_TO_TICKS(1000)) == pdPASS)
            {
                if (request.eDataID == 1)
                {
                    printf("func_1 ID: %d, Data: %d.\n", request.eDataID, request.lDataValue);
                }
                else
                {
                    printf("error func_1 !!!\n");
                    func_2(request);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    else
        printf("Queue is empty.\n");

    return;
}

void app_main()
{
    qHandler = xQueueCreate(5, sizeof(Data_t));
    xTaskCreate(reception, "reception", 2048, NULL, 2, NULL);
    xTaskCreate(func_1, "function_1", 2048, NULL, 1, NULL);
}