#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>

// Define the LED0_NODE alias for the onboard LED
// #define LED0_NODE DT_ALIAS(led0)
#define STATUS_LED_NODE DT_NODELABEL(status_led)
#define BUTTON_LED_NODE DT_NODELABEL(button_led)

extern int blink_period;
extern struct k_work_delayable led_work;
extern const struct gpio_dt_spec status_led;
extern const struct gpio_dt_spec button_led;

// Function declarations
int init_leds(void);
void led_blink_work(struct k_work *work);
void stop_blinking(void);
int cmd_led_on(const struct shell *sh, size_t argc, char **argv);
int cmd_led_off(const struct shell *sh, size_t argc, char **argv);
int cmd_led_blink(const struct shell *sh, size_t argc, char **argv);
#endif // LED_CONTROL_H
