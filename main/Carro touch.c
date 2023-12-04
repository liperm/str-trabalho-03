#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "driver/touch_pad.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "esp_timer.h"

#include <unistd.h>

static const char *TAG = "Touch pad";

#define TOUCH_THRESH_NO_USE (0)
#define TOUCH_THRESH_PERCENT (80)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

static bool s_pad_activated[TOUCH_PAD_MAX];
static uint32_t s_pad_init_val[TOUCH_PAD_MAX];

/*
1 - ACCELERATE = D13,
2 - BREAK = D12,
3 - AIRBAG = D14
4 - BELT = D27,
5 - LOCK = D33,
6 - HOT = D32,
7 - WINDOW = D4,
8 - LIGHTS = D15,
*/
enum sensors
{
    ACCELERATE = 4,
    BREAK = 5,
    AIRBAG = 6,
    BELT = 7,
    LOCK = 8,
    HOT = 9,
    WINDOW = 0,
    LIGHTS = 3,
};

static void thread_motor(void *pvParameter)
{
    while (1)
    {
        // usleep(11);
        if (s_pad_activated[ACCELERATE] == true)
        {
            // clock_gettime(CLOCK_MONOTONIC, &clock_accelerate_start);
            s_pad_activated[ACCELERATE] = false;
            // clock_gettime(CLOCK_MONOTONIC, &ACCELERATE_CLOCK_END);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static void thread_abs(void *pvParameter)
{
    while (1)
    {
        // usleep(11);
        if (s_pad_activated[BREAK] == true)
        {
            // clock_gettime(CLOCK_MONOTONIC, &ABS_CLOCK_START);
            s_pad_activated[BREAK] = false;
            // clock_gettime(CLOCK_MONOTONIC, &ABS_CLOCK_END);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static void thread_airbag(void *pvParameter)
{
    while (1)
    {
        if (s_pad_activated[AIRBAG] == true)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            s_pad_activated[AIRBAG] = false;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static void thread_belt(void *pvParameter)
{
    while (1)
    {
        if (s_pad_activated[BELT] == true)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            s_pad_activated[BELT] = false;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static void thread_lvt(void *pvParameter)
{
    while (1)
    {
        if (s_pad_activated[LOCK] == true)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            s_pad_activated[LOCK] = false;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }
        if (s_pad_activated[WINDOW] == true)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            s_pad_activated[WINDOW] = false;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }
        if (s_pad_activated[LIGHTS] == true)
        {
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_START);
            s_pad_activated[LIGHTS] = false;
            // clock_gettime(CLOCK_MONOTONIC, &AIRBAG_CLOCK_END);
        }

        vTaskDelay(90 / portTICK_PERIOD_MS);
    }
}

/*
  Read values sensed at all available touch pads.
  Use 2 / 3 of read value as the threshold
  to trigger interrupt when the pad is touched.
  Note: this routine demonstrates a simple way
  to configure activation threshold for the touch pads.
  Do not touch any pads when this routine
  is running (on application start).
 */
static void tp_example_set_thresholds(void)
{
    uint16_t touch_value;
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
    {
        // read filtered value
        touch_pad_read_filtered(i, &touch_value);
        s_pad_init_val[i] = touch_value;
        ESP_LOGI(TAG, "test init: touch pad [%d] val is %d", i, touch_value);
        // set interrupt threshold.
        ESP_ERROR_CHECK(touch_pad_set_thresh(i, touch_value * 2 / 3));
    }
}

static void read_touch(void *pvParameter)
{   printf("\033[2J\033[H");

    while (1)
    {
        // interrupt mode, enable touch interrupt
        touch_pad_intr_enable();
        printf("\e[1;1H");
        if (s_pad_activated[ACCELERATE] == true)
        {
            printf("\x1b[32m ACCELERATE\n");
        }else{
            printf("\x1b[90m ACCELERATE\n");
        }
        if (s_pad_activated[BREAK] == true)
        {
            printf("\x1b[32m BREAK\n");
        }else{
            printf("\x1b[90m BREAK\n");
        }
        
        if (s_pad_activated[AIRBAG] == true)
        {
            printf("\x1b[32m AIRBAG\n");
        }else{
            printf("\x1b[90m AIRBAG\n");
        }
        if (s_pad_activated[BELT] == true)
        {
            printf("\x1b[32m BELT\n");
        }else{
            printf("\x1b[90m BELT\n");
        }
        if (s_pad_activated[LOCK] == true)
        {
            printf("\x1b[32m LOCK\n");
        }else{
            printf("\x1b[90m LOCK\n");
        }
        if (s_pad_activated[WINDOW] == true)
        {
            printf("\x1b[32m WINDOW\n");
        }else{
            printf("\x1b[90m WINDOW\n");
        }
        if (s_pad_activated[LIGHTS] == true)
        {
            printf("\x1b[32m LIGHTS\n");
        }else{
            printf("\x1b[90m LIGHTS\n");
        }
        // else if (s_pad_activated[HOT] == true)
        // {
        //     printf("\nHOT");
        //     // Clear information on pad activation
        // }

        
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

static void tp_example_rtc_intr(void *arg)
{
    uint32_t pad_intr = touch_pad_get_status();
    // clear interrupt
    touch_pad_clear_status();
    if ((pad_intr >> ACCELERATE) & 0x01)
    {
        s_pad_activated[ACCELERATE] = true;
    }
    if ((pad_intr >> BREAK) & 0x01)
    {
        s_pad_activated[BREAK] = true;
    }
    if ((pad_intr >> AIRBAG) & 0x01)
    {
        s_pad_activated[AIRBAG] = true;
    }
    if ((pad_intr >> BELT) & 0x01)
    {
        s_pad_activated[BELT] = true;
    }
    if ((pad_intr >> LOCK) & 0x01)
    {
        s_pad_activated[LOCK] = true;
    }
    if ((pad_intr >> HOT) & 0x01)
    {
        s_pad_activated[HOT] = true;
    }
    if ((pad_intr >> WINDOW) & 0x01)
    {
        s_pad_activated[WINDOW] = true;
    }
    if ((pad_intr >> LIGHTS) & 0x01)
    {
        s_pad_activated[LIGHTS] = true;
    }
}

/*
 * Before reading touch pad, we need to initialize the RTC IO.
 */
static void tp_example_touch_pad_init(void)
{
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
    {
        // init RTC IO and mode for touch pad.
        touch_pad_config(i, TOUCH_THRESH_NO_USE);
    }
}

void app_main(void)
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing touch pad");
    touch_pad_init();
    // If use interrupt trigger mode, should set touch sensor FSM mode at 'TOUCH_FSM_MODE_TIMER'.
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // Set reference voltage for charging/discharging
    // For most usage scenarios, we recommend using the following combination:
    // the high reference valtage will be 2.7V - 1V = 1.7V, The low reference voltage will be 0.5V.
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    // Init touch pad IO
    tp_example_touch_pad_init();
    // Initialize and start a software filter to detect slight change of capacitance.
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    // Set thresh hold
    tp_example_set_thresholds();
    // Register touch interrupt ISR
    touch_pad_isr_register(tp_example_rtc_intr, NULL);
    // Start a task to show what pads have been touched
    xTaskCreate(&read_touch, "touch_pad_read_task", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_motor, "thread_motor", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_abs, "thread_abs", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_airbag, "thread_airbag", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_belt, "thread_belt", 2048, NULL, 5, NULL);
    xTaskCreate(&thread_lvt, "thread_lock", 2048, NULL, 5, NULL);
}