#include "button_control.h"

//// ----------- BUTTON CONTROL -------------------------------------------------------
LOG_MODULE_REGISTER(buttonctrl, CONFIG_LOG_DEFAULT_LEVEL); // Registers the log level for the module
/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
                                                       {0});
struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb,
                    uint32_t pins)
{
    LOG_INF("Button pressed at %" PRIu32 "", k_cycle_get_32());
}

// Initialise button
int init_button()
{
    int ret;
    if (!gpio_is_ready_dt(&button))
    {
        LOG_ERR("Error: button device %s is not ready\n",
                button.port->name);
        return 1;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0)
    {
        LOG_ERR("Error %d: failed to configure %s pin %d",
                ret, button.port->name, button.pin);
        return 1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button,
                                          GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0)
    {
        LOG_ERR("Error %d: failed to configure interrupt on %s pin %d",
                ret, button.port->name, button.pin);
        return 1;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    LOG_INF("Set up button at %s pin %d", button.port->name, button.pin);
    return 0;
}