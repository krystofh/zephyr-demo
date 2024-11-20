#include "button_control.h"

//// ----------- BUTTON CONTROL -------------------------------------------------------
#define DEBOUNCE_DELAY_MS 15                               // Debounce delay in milliseconds
LOG_MODULE_REGISTER(buttonctrl, CONFIG_LOG_DEFAULT_LEVEL); // Registers the log level for the module

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
                                                       {0});
struct gpio_callback button_cb_data;
static bool button_state = 0;
// Declare kernel work item
static struct k_work_delayable button_work;

static char *helper_button_evt_str(enum button_evt evt)
{
    switch (evt)
    {
    case BUTTON_EVT_PRESSED:
        return "Pressed";
    case BUTTON_EVT_RELEASED:
        return "Released";
    default:
        return "Unknown";
    }
}

// Function to handle the work (debounce timer expiration)
static void button_work_handler(struct k_work *work)
{
    // Set button_pressed flag
    ARG_UNUSED(work);
    int val = gpio_pin_get_dt(&button);
    enum button_evt evt = val ? BUTTON_EVT_PRESSED : BUTTON_EVT_RELEASED;
    LOG_INF("Button event: %s\n", helper_button_evt_str(evt));
}

void button_isr(const struct device *dev, struct gpio_callback *cb,
                uint32_t pins)
{
    // LOG_INF("Button pressed at %" PRIu32 "", k_cycle_get_32());
    // Check if work queue has some pending work (button already being debounced)
    if (k_work_delayable_is_pending(&button_work))
    {
        LOG_INF("Rescheduling debounce timer due to edge detection.");
        // Cancel ongoing work and reschedule debounce timer
        k_work_cancel_delayable(&button_work);
    }
    else
    {
        LOG_INF("Starting debounce timer.");
    }
    // Reschedule debounce work for DEBOUNCE_DELAY_MS
    k_work_reschedule(&button_work, K_MSEC(DEBOUNCE_DELAY_MS));
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
                                          GPIO_INT_EDGE_BOTH);
    if (ret != 0)
    {
        LOG_ERR("Error %d: failed to configure interrupt on %s pin %d",
                ret, button.port->name, button.pin);
        return 1;
    }

    gpio_init_callback(&button_cb_data, button_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    button_state = gpio_pin_get_dt(&button);
    // Initialize delayed work
    k_work_init_delayable(&button_work, button_work_handler);
    LOG_INF("Set up button at %s pin %d", button.port->name, button.pin);
    return 0;
}