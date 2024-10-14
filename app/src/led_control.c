#include "led_control.h"

/// -----------  LED CONTROL -------------------------------------------------------
LOG_MODULE_REGISTER(ledctrl, CONFIG_LOG_DEFAULT_LEVEL); // Registers the log level for the module

// Init work items
int blink_period = 500; // in ms
struct k_work_delayable led_work;

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios); // onboard LED device
// static struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(DT_NODELABEL(red_led), gpios); // red LED

void led_blink_work(struct k_work *work)
{
    // Toggle LED
    int ret;
    ret = gpio_pin_toggle_dt(&led);
    // Reschedule the work to be done again in half of the period ms
    k_work_reschedule(&led_work, K_MSEC(blink_period / 2)); // provide time as k_ticks_t using the macro for conversion
}

// Stop the blink work
void stop_blinking()
{
    int ret = k_work_cancel_delayable(&led_work);
    if (ret == 0)
    {
        LOG_INF("Blinking stopped successfully.\n");
    }
    else if (ret == -EALREADY)
    {
        LOG_INF("Blinking was already stopped.\n"); // delete if not needed
    }
    else
    {
        LOG_ERR("Failed to stop blinking.\n");
    }
}

int cmd_led_on(const struct shell *sh, size_t argc, char **argv)
{
    // Stop blinking first
    stop_blinking();

    // Turn ON LED
    int response = gpio_pin_set_dt(&led, 1);
    if (response < 0)
    {
        return 1;
    }
    LOG_INF("LED ON\n");
    return 0;
}

int cmd_led_off(const struct shell *sh, size_t argc, char **argv)
{
    // Stop blinking first
    stop_blinking();
    // Turn OFF LED
    int response = gpio_pin_set_dt(&led, 0);
    if (response < 0)
    {
        return 1;
    }
    LOG_INF("LED OFF\n");
    return 0;
}

// Blink the led with a set period in ms
int cmd_led_blink(const struct shell *sh, size_t argc, char **argv)
{
    // TODO handle negative numbers or no numbers at all
    blink_period = atoi(argv[1]); // store the argument in global variable
    if (blink_period > 0)
    {
        stop_blinking(); // stop previous work processing to react to new command exec
        LOG_INF("Starting to blink with period %d ms\n", blink_period);
        k_work_init_delayable(&led_work, led_blink_work);
        k_work_schedule(&led_work, K_NO_WAIT); // schedule the first execution to start immediately
    }
    else
    {
        LOG_ERR("Setting blink period not successful\n");
        return 1;
    };
    return 0;
}

// Register commands
SHELL_CMD_REGISTER(led_on, NULL, "Turn the LED ON", cmd_led_on);
SHELL_CMD_REGISTER(led_off, NULL, "Turn the LED OFF", cmd_led_off);
SHELL_CMD_ARG_REGISTER(led_blink, NULL, "Blink the LED with a defined period in ms.", cmd_led_blink, 2, NULL); // 2 mandatory args - the command name and ther period