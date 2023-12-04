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
#define PINO_D2 2
#define PINO_D4 4
#define PINO_D16 16
#define PINO_D17 17
#define PINO_D5 5
#define PINO_D18 18
#define PINO_D19 19
#define PINO_D21 21

SemaphoreHandle_t xSemaphore;

int64_t   TA;
int64_t   TB;
int64_t   TAIR;
int64_t   TBE;
int64_t   TLO;
int64_t   TW;
int64_t   TL;
int64_t   TH;

static int leds[] = {PINO_D2, PINO_D4, PINO_D16, PINO_D17, PINO_D5, PINO_D18, PINO_D19,PINO_D21};
static int pinos[] = {PINO_D13, PINO_D12, PINO_D14, PINO_D27, PINO_D26, PINO_D25, PINO_D33, PINO_D32,PINO_D35 };
static bool estados[9] = {false};
bool  all = true;
float velocidade = 0;
float  velocidadeM = 0;
float  velocidadeS = 0;
int contador = 0;
int consumo = 0; 
int consumoS = 0; 
int consumoM = 0;

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
void configurar_leds() {
    gpio_config_t io_conf;

    // Lista de pinos a serem configurados como saídas
    int num_pinos = sizeof(leds) / sizeof(leds[0]);

    // Configuração dos pinos como saídas
    for (int i = 0; i < num_pinos; i++) {
        io_conf.pin_bit_mask = (1ULL << leds[i]);
        io_conf.mode = GPIO_MODE_OUTPUT; // Configuração para saída
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE; // Desabilita pull-up
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // Desabilita pull-down
        gpio_config(&io_conf);
    }
}


static void thread_motor(void *pvParameter)
{
    while (1)
    {   
        
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            contador += 1;
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[ACCELERATE]);
            if (aux == false)
            {   
                usleep(490);

                estados[ACCELERATE] = true;
                gpio_set_level(leds[ACCELERATE], 1);
                if (velocidade<150){
                    velocidade += 0.1;
                }
                
                
            }else{
                estados[ACCELERATE] = false;
                gpio_set_level(leds[ACCELERATE], 0);
            }
            int64_t fim = esp_timer_get_time();
            TA = fim - inicio;
            if(velocidade > 0 ){
                if(aux == false ){
                    consumo = 1;
                }else{
                    if(velocidade <20){
                        consumo = 8;
                    }else if(velocidade <90){
                        consumo = 10;
                    }else if(velocidade <110){
                        consumo = 14;
                    }else if(velocidade <150){
                        consumo = 12;
                    }
                }  
                consumoS += consumo;
                consumoM = consumoS/contador; 
            }else{
                consumo = 0;
            }
            velocidadeS += velocidade;
            velocidadeM =  velocidadeS/contador;
            
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        
    }
}


static void thread_abs(void *pvParameter)
{
    while (1)
    {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[BREAK]);
            if ( aux == 0)
            {
                usleep(100000);
                estados[BREAK] = true;
                if ((velocidade - 5)<0){
                    velocidade = 0;
                }else if (velocidade>0){
                    velocidade -= 5;
                }
                gpio_set_level(leds[BREAK], 1);
                
            }else{
                estados[BREAK] = false;
                gpio_set_level(leds[BREAK], 0);
            }
            int64_t fim = esp_timer_get_time();
            TB = fim - inicio;
            
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_airbag(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[AIRBAG]);
            if ( aux== 0)
            {
                usleep(100000);
                estados[AIRBAG] = true;
                gpio_set_level(leds[AIRBAG], 1);
            }else{
                estados[AIRBAG] = false;
                gpio_set_level(leds[AIRBAG], 0);
            }
            int64_t fim = esp_timer_get_time();
            TAIR = fim - inicio;
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_belt(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[BELT]);
            if ( aux== 0)
            {
                usleep(1000000);
                estados[BELT] = true;
                gpio_set_level(leds[BELT], 1);
            }else{
                estados[BELT] = false;
                gpio_set_level(leds[BELT], 0);
            }
            int64_t fim = esp_timer_get_time();
            TBE = fim - inicio;
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_lock(void *pvParameter)
{
    while (1)
    {  
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[LOCK]);
            if ( aux == 0)
            {
                usleep(1000000);
                estados[LOCK] = true;
                gpio_set_level(leds[LOCK], 1);
            }else{
                estados[LOCK] = false;
                gpio_set_level(leds[LOCK], 0);
            }
            int64_t fim = esp_timer_get_time();
            TLO = fim - inicio;
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        
    }
}

static void thread_window(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[WINDOW]);

            if ( aux== 0)
            {
                usleep(1000000);
                estados[WINDOW] = true;
                gpio_set_level(leds[WINDOW], 1);
            }else{
                estados[WINDOW] = false;
                gpio_set_level(leds[WINDOW], 0);
            }
            int64_t fim = esp_timer_get_time();
            TW = fim - inicio;
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_lights(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[LIGHTS]);
            if ( aux == 0)
            {
                usleep(1000000);
                estados[LIGHTS] = true;
                gpio_set_level(leds[LIGHTS], 1);
            }else{
                estados[LIGHTS] = false;
                gpio_set_level(leds[LIGHTS], 0);
            }
            int64_t fim = esp_timer_get_time();
            TL = fim - inicio;
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void thread_hot(void *pvParameter)
{
    while (1)
    {   
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            int64_t inicio = esp_timer_get_time();
            bool aux = gpio_get_level(pinos[HOT]);
            if ( aux== 0)
            {
                usleep(20000);
                estados[HOT] = true;
                gpio_set_level(leds[HOT], 1);
            }else{
                estados[HOT] = false;
                gpio_set_level(leds[HOT], 0);
            }
            int64_t fim = esp_timer_get_time();
            TH = fim - inicio;
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void read_touch(void *pvParameter)
{   printf("\033[2J\033[H");

    while (1)
    {
        // interrupt mode, enable touch interrupt
        //touch_pad_intr_enable();
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {

            printf("\e[1;1H\x1b[50m Velocidade [%d]Km/H ~ [%d](Km/H)/T \e[1;37H Consumo [%d]Km/L ~ [%d](Km/L)/T \n", (int)velocidade, (int)velocidadeM, consumo, consumoM);

            if (estados[ACCELERATE] == true)
            {
                printf("\e[2;1H\x1b[32m ACCELERATE\n" );
            }else{
                printf("\e[2;1H\x1b[90m ACCELERATE\n");
            }
            if (estados[BREAK] == true)
            {
                printf("\e[3;1H\x1b[32m BREAK\n");
            }else{
                printf("\e[3;1H\x1b[90m BREAK\n");
            }
            
            if (estados[AIRBAG] == true)
            {
                printf("\e[4;1H\x1b[32m AIRBAG\n");
            }else{
                printf("\e[4;1H\x1b[90m AIRBAG\n");
            }
            if (estados[BELT] == true)
            {
                printf("\e[5;1H\x1b[32m BELT\n");
            }else{
                printf("\e[5;1H\x1b[90m BELT\n");
            }
            if (estados[LOCK] == true)
            {
                printf("\e[6;1H\x1b[32m LOCK\n");
            }else{
                printf("\e[6;1H\x1b[90m LOCK\n");
            }
            if (estados[WINDOW] == true)
            {
                printf("\e[7;1H\x1b[32m WINDOW\n");
            }else{
                printf("\e[7;1H\x1b[90m WINDOW\n");
            }
            if (estados[LIGHTS] == true)
            {
                printf("\e[8;1H\x1b[32m LIGHTS\n");
            }else{
                printf("\e[8;1H\x1b[90m LIGHTS\n");
            }
            if (estados[HOT] == true)
            {
                printf("\e[9;1H\x1b[32m HOT\x1b[90m\n");
            }else{
                printf("\e[9;1H\x1b[90m HOT\n");
            }
            
            printf("\e[2;14H %lld us            ",TA);
            printf("\e[3;14H %lld us            ",TB);
            printf("\e[4;14H %lld us            ",TAIR);
            printf("\e[5;14H %lld us            ",TBE);
            printf("\e[6;14H %lld us            ",TLO);
            printf("\e[7;14H %lld us            ",TW);
            printf("\e[8;14H %lld us            ",TL);
            printf("\e[9;14H %lld us            \n",TH);
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void app_main() {
    configurar_pullups();
    configurar_leds();
    // Inicializa o semáforo
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemaphore); // Libera o semáforo pela primeira vez
    
    // As tarefas são criadas e associadas a um núcleo específico com uma prioridade definida
    xTaskCreatePinnedToCore(&read_touch, "touch_pad_read_task", 2048, NULL, 99, NULL, 0);

    xTaskCreatePinnedToCore(&thread_motor, "thread_motor", 2048, NULL, 99, NULL, 0);
    xTaskCreatePinnedToCore(&thread_hot, "thread_hot", 2048, NULL, 99, NULL, 0);
    xTaskCreatePinnedToCore(&thread_abs, "thread_abs", 2048, NULL, 99, NULL, 0);
    xTaskCreatePinnedToCore(&thread_airbag, "thread_airbag", 2048, NULL, 99, NULL, 0);

    xTaskCreatePinnedToCore(&thread_belt, "thread_belt", 2048, NULL, 98, NULL, 0);
    xTaskCreatePinnedToCore(&thread_lock, "thread_lock", 2048, NULL, 98, NULL, 0);
    xTaskCreatePinnedToCore(&thread_window, "thread_window", 2048, NULL, 98, NULL, 0);
    xTaskCreatePinnedToCore(&thread_lights, "thread_lights", 2048, NULL, 98, NULL, 0);


}



