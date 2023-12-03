#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/semphr.h"
#include <unistd.h>

#define PINO_D13 13
#define PINO_D12 12
#define PINO_D14 14
#define PINO_D27 27
#define PINO_D26 26
#define PINO_D25 25
#define PINO_D33 33
#define PINO_D32 32
#define PINO_D35 23

SemaphoreHandle_t xMutex;
StaticSemaphore_t xMutexBuffer;

int64_t   TA;
int64_t   TB;
int64_t   TAIR;
int64_t   TBE;
int64_t   TLO;
int64_t   TW;
int64_t   TL;
int64_t   TH;


static int pinos[] = {PINO_D13, PINO_D12, PINO_D14, PINO_D27, PINO_D26, PINO_D25, PINO_D33, PINO_D32,PINO_D35 };
static bool estados[9] = {false};
bool  all = true;

enum sensors
{
    ACCELERATE = 0,
    BREAK = 1,
    AIRBAG = 2,
    BELT = 3,
    LOCK = 4,
    WINDOW = 5,
    LIGHTS = 6,
    HOT = 7,
    ALL = 8

};

void configurar_pullups() {
    gpio_config_t io_conf;

    // Lista de pinos a serem configurados com pull-up
    int num_pinos = sizeof(pinos) / sizeof(pinos[0]);

    // Configuração dos pinos como entradas com pull-up
    for (int i = 0; i < num_pinos; i++) {
        io_conf.pin_bit_mask = (1ULL << pinos[i]);
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_config(&io_conf);
   }
}


static void thread_motor(void *pvParameter)
{
    while (1)
    {   
        
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[ACCELERATE]);
            if (aux == false)
            { 
                usleep(500);
                estados[ACCELERATE] = true;
            }else{
                estados[ACCELERATE] = false;
            }
            int64_t fim = esp_timer_get_time();
            TA = fim - inicio;
             xSemaphoreGive(xMutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        
    }
}

static void thread_abs(void *pvParameter)
{
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[BREAK]);
            if ( aux == 0)
            {
                usleep(100000);
                estados[BREAK] = true;
            }else{
                estados[BREAK] = false;
            }
            int64_t fim = esp_timer_get_time();
            TB = fim - inicio;
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_airbag(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[AIRBAG]);
            if ( aux== 0)
            {
                usleep(100000);
                estados[AIRBAG] = true;
            }else{
                estados[AIRBAG] = false;
            }
            int64_t fim = esp_timer_get_time();
            TAIR = fim - inicio;
             xSemaphoreGive(xMutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_belt(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[BELT]);
            if ( aux== 0)
            {
                usleep(1000000);
                estados[BELT] = true;
            }else{
                estados[BELT] = false;
            }
            int64_t fim = esp_timer_get_time();
            TBE = fim - inicio;
             xSemaphoreGive(xMutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_lock(void *pvParameter)
{
    while (1)
    {  
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[LOCK]);
            if ( aux == 0)
            {
                usleep(1000000);
                estados[LOCK] = true;
            }else{
                estados[LOCK] = false;
            }
            int64_t fim = esp_timer_get_time();
            TLO = fim - inicio;
             xSemaphoreGive(xMutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        
    }
}

static void thread_window(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[WINDOW]);

            if ( aux== 0)
            {
                usleep(1000000);
                estados[WINDOW] = true;
            }else{
                estados[WINDOW] = false;
            }
            int64_t fim = esp_timer_get_time();
            TW = fim - inicio;
             xSemaphoreGive(xMutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_lights(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[LIGHTS]);
            if ( aux == 0)
            {
                usleep(1000000);
                estados[LIGHTS] = true;
            }else{
                estados[LIGHTS] = false;
            }
            int64_t fim = esp_timer_get_time();
            TL = fim - inicio;
             xSemaphoreGive(xMutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_hot(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[HOT]);
            if ( aux== 0)
            {
                usleep(20000);
                estados[HOT] = true;
            }else{
                estados[HOT] = false;
            }
            int64_t fim = esp_timer_get_time();
            TH = fim - inicio;
             xSemaphoreGive(xMutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void read_touch(void *pvParameter)
{   printf("\033[2J\033[H");
    printf("ACCELERATE;BREAK;AIRBAG;BELT;LOCK;WINDOW;LIGHTS;HOT\n");
    while (1)
    {
        printf("%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld\n",TA,TB,TAIR,TBE,TLO,TW,TL,TH);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void app_main() {
    configurar_pullups();

    // Cria um mutex com herança de prioridade usando um buffer estático
    xMutex = xSemaphoreCreateMutexStatic(&xMutexBuffer);

    // As tarefas são criadas e associadas a um núcleo específico com uma prioridade definida
    xTaskCreatePinnedToCore(&read_touch, "touch_pad_read_task", 2048, NULL, 99, NULL,0);
    xTaskCreatePinnedToCore(&thread_motor, "thread_motor", 2048, NULL, 98, NULL,0);
    xTaskCreatePinnedToCore(&thread_abs, "thread_abs", 2048, NULL, 98, NULL,0);
    xTaskCreatePinnedToCore(&thread_airbag, "thread_airbag", 2048, NULL, 98, NULL,0);
    xTaskCreatePinnedToCore(&thread_belt, "thread_belt", 2048, NULL, 99, NULL,0);
    xTaskCreatePinnedToCore(&thread_lock, "thread_lock", 2048, NULL, 99, NULL,0);
    xTaskCreatePinnedToCore(&thread_window, "thread_window", 2048, NULL, 99, NULL,0);
    xTaskCreatePinnedToCore(&thread_lights, "thread_lights", 2048, NULL, 99, NULL,0);
    xTaskCreatePinnedToCore(&thread_hot, "thread_hot", 2048, NULL, 98, NULL,0);

}