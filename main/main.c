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
ACCELERATE = D13
BREAK = D14
*/
enum actions
{
    ACCELERATE = 4,
    BREAK = 6
};

static void thread_motor(void *pvParameter)
{
    while (1)
    {
        usleep(11);
        if (s_pad_activated[ACCELERATE] == true)
        {
            printf("VRUUUUUMMMM");
            // clock_gettime(CLOCK_MONOTONIC, &clock_accelerate_start);
            usleep(16);
            s_pad_activated[ACCELERATE] = false;
            // clock_gettime(CLOCK_MONOTONIC, &ACCELERATE_CLOCK_END);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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
{
    while (1)
    {
        // interrupt mode, enable touch interrupt
        touch_pad_intr_enable();
        if (s_pad_activated[ACCELERATE] == true)
        {
            printf("\nAccelerate");
            
            // Clear information on pad activation
        }
        else if (s_pad_activated[BREAK] == true)
        {
            printf("\nBreak");

            // Clear information on pad activation
            s_pad_activated[BREAK] = false;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    else if ((pad_intr >> BREAK) & 0x01)
    {
        s_pad_activated[BREAK] = true;
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
}
