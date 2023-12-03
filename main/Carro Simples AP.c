#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#define PINO_D13 13
#define PINO_D12 12
#define PINO_D14 14
#define PINO_D27 27
#define PINO_D26 26
#define PINO_D25 25
#define PINO_D33 33
#define PINO_D32 32
#define PINO_D35 4
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

static void thread_all(void *pvParameter)
{
    while (1)
    {   

        all = true;
        if(gpio_get_level(pinos[ALL]) == 0){
                    all = false;
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_motor(void *pvParameter)
{
    while (1)
    {   
        int64_t inicio = esp_timer_get_time();
        bool aux = gpio_get_level(pinos[ACCELERATE]);
        if ((all && aux) == false)
        {   int resultado = 0;
            for (int i = 0; i < 6150; i++) {
                resultado += i * i; // Exemplo de operação que consome tempo
            }
            estados[ACCELERATE] = true;
        }else{
            estados[ACCELERATE] = false;
        }
        int64_t fim = esp_timer_get_time();
        TA = fim - inicio;
        vTaskDelay(10 / portTICK_PERIOD_MS);
        
    }
}

static void thread_abs(void *pvParameter)
{
    while (1)
    {
        
        int64_t inicio = esp_timer_get_time();
        bool aux = gpio_get_level(pinos[BREAK]);
        // usleep(11);
        if ((all && aux) == 0)
        {
            // clock_gettime(CLOCK_MONOTONIC, &ABS_CLOCK_START);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            estados[BREAK] = true;
            // clock_gettime(CLOCK_MONOTONIC, &ABS_CLOCK_END);
        }else{
            estados[BREAK] = false;
        }
        int64_t fim = esp_timer_get_time();
        TB = fim - inicio;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_airbag(void *pvParameter)
{
    while (1)
    {   
        int64_t inicio = esp_timer_get_time();
        bool aux = gpio_get_level(pinos[AIRBAG]);
        if ((all && aux)== 0)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            estados[AIRBAG] = true;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }else{
            estados[AIRBAG] = false;
        }
        int64_t fim = esp_timer_get_time();
        TAIR = fim - inicio;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_belt(void *pvParameter)
{
    while (1)
    {   
        int64_t inicio = esp_timer_get_time();
        bool aux = gpio_get_level(pinos[BELT]);
        if ((all && aux)== 0)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            estados[BELT] = true;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }else{
            estados[BELT] = false;
        }
        int64_t fim = esp_timer_get_time();
        TBE = fim - inicio;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_lock(void *pvParameter)
{
    while (1)
    {  
        int64_t inicio = esp_timer_get_time();
        bool aux = gpio_get_level(pinos[LOCK]);
        if ((all && aux) == 0)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            estados[LOCK] = true;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
            
        }else{
             estados[LOCK] = false;
        }
        int64_t fim = esp_timer_get_time();
        TLO = fim - inicio;
        vTaskDelay(10 / portTICK_PERIOD_MS);
        
    }
}

static void thread_window(void *pvParameter)
{
    while (1)
    {   
        int64_t inicio = esp_timer_get_time();
        bool aux = gpio_get_level(pinos[WINDOW]);
        if ( (all && aux)== 0)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            estados[WINDOW] = true;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }else{
            estados[WINDOW] = false;
        }
        int64_t fim = esp_timer_get_time();
        TW = fim - inicio;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_lights(void *pvParameter)
{
    while (1)
    {   
        int64_t inicio = esp_timer_get_time();
        bool aux = gpio_get_level(pinos[LIGHTS]);
        if ((all && aux) == 0)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            estados[LIGHTS] = true;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }else{
            estados[LIGHTS] = false;
        }
        int64_t fim = esp_timer_get_time();
        TL = fim - inicio;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_hot(void *pvParameter)
{
    while (1)
    {   
        int64_t inicio = esp_timer_get_time();
        bool aux = gpio_get_level(pinos[HOT]);
        if ((all && aux)== 0)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            vTaskDelay(20 / portTICK_PERIOD_MS);
            estados[HOT] = true;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }else{
            estados[HOT] = false;
        }
        int64_t fim = esp_timer_get_time();
        TH = fim - inicio;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void read_touch(void *pvParameter)
{   printf("\033[2J\033[H");

    while (1)
    {
        // interrupt mode, enable touch interrupt
        //touch_pad_intr_enable();
        printf("\e[1;1H");
        if (estados[ACCELERATE] == true)
        {
            printf("\x1b[32m ACCELERATE\n" );
        }else{
            printf("\x1b[90m ACCELERATE\n");
        }
        if (estados[BREAK] == true)
        {
            printf("\x1b[32m BREAK\n");
        }else{
            printf("\x1b[90m BREAK\n");
        }
        
        if (estados[AIRBAG] == true)
        {
            printf("\x1b[32m AIRBAG\n");
        }else{
            printf("\x1b[90m AIRBAG\n");
        }
        if (estados[BELT] == true)
        {
            printf("\x1b[32m BELT\n");
        }else{
            printf("\x1b[90m BELT\n");
        }
        if (estados[LOCK] == true)
        {
            printf("\x1b[32m LOCK\n");
        }else{
            printf("\x1b[90m LOCK\n");
        }
        if (estados[WINDOW] == true)
        {
            printf("\x1b[32m WINDOW\n");
        }else{
            printf("\x1b[90m WINDOW\n");
        }
        if (estados[LIGHTS] == true)
        {
            printf("\x1b[32m LIGHTS\n");
        }else{
            printf("\x1b[90m LIGHTS\n");
        }
        if (estados[HOT] == true)
        {
            printf("\x1b[32m HOT\n");
        }else{
            printf("\x1b[90m HOT\n");
        }
        
        printf("\e[1;14H %lld us            ",TA);
        printf("\e[2;14H %lld us            ",TB);
        printf("\e[3;14H %lld us            ",TAIR);
        printf("\e[4;14H %lld us            ",TBE);
        printf("\e[5;14H %lld us            ",TLO);
        printf("\e[6;14H %lld us            ",TW);
        printf("\e[7;14H %lld us            ",TL);
        printf("\e[8;14H %lld us            ",TH);


        
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    configurar_pullups();

    xTaskCreate(&read_touch, "touch_pad_read_task", 2048, NULL, 5, NULL);
    xTaskCreate(&read_touch, "touch_pad_read_task", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_motor, "thread_motor", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_abs, "thread_abs", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_airbag, "thread_airbag", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_belt, "thread_belt", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_lock, "thread_lock", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_window, "thread_lock", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_lights, "thread_lock", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_hot, "thread_lock", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_all, "thread_lock", 2048, NULL, 5, NULL);
}
