#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <inttypes.h>

#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

extern const struct gpio_dt_spec button;
extern struct gpio_callback button_cb_data;

enum button_evt
{
    BUTTON_EVT_PRESSED,
    BUTTON_EVT_RELEASED
};

// Function declarations
static char *helper_button_evt_str(enum button_evt evt);
void button_pressed(const struct device *dev, struct gpio_callback *cb,
                    uint32_t pins);
int init_button(void);

#endif // BUTTON_CONTROL_H