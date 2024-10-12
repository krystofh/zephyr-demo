/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Zephyr-specific libs
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>

// Shell libs
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/version.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 500

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/*
 * Ensure that an overlay for USB serial has been defined.
 */
BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
			 "Console device is not ACM CDC UART device");

LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL); // registers the log level for the module MAIN specified in Kconfig

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios); // LED device

// Example shell command handler
// Prints "pong" as a response to "ping" request
static int cmd_demo_ping(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "pong");

	return 0;
}

// Print out command with provided arguments
static int cmd_demo_params(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "argc = %zd", argc);
	for (size_t cnt = 0; cnt < argc; cnt++)
	{
		shell_print(sh, "  argv[%zd] = %s", cnt, argv[cnt]);
	}

	return 0;
}

/// -----------  LED CONTROL -------------------------------------------------------

// Init work items
static int blink_period = 500; // in ms
static struct k_work_delayable led_work;

static void led_blink_work(struct k_work *work)
{
	// Toggle LED
	int ret;
	ret = gpio_pin_toggle_dt(&led);
	// Reschedule the work to be done again in half of the period ms
	k_work_reschedule(&led_work, K_MSEC(blink_period / 2)); // provide time as k_ticks_t using the macro for conversion
}

// Stop the blink work
static void stop_blinking()
{
	int ret = k_work_cancel_delayable(&led_work);
	if (ret == 0)
	{
		printk("Blinking stopped successfully.\n");
		LOG_INF("log example");
	}
	else if (ret == -EALREADY)
	{
		printk("Blinking was already stopped.\n");
	}
	else
	{
		printk("Failed to stop blinking.\n");
	}
}

static int cmd_led_on(const struct shell *sh, size_t argc, char **argv)
{
	// Stop blinking first
	stop_blinking();
	// Turn ON LED
	return (gpio_pin_set_dt(&led, 1) < 0) ? 1 : (shell_print(sh, "LED on"), 0);
}

static int cmd_led_off(const struct shell *sh, size_t argc, char **argv)
{
	// Stop blinking first
	stop_blinking();
	// Turn OFF LED
	return (gpio_pin_set_dt(&led, 0) < 0) ? 1 : (shell_print(sh, "LED OFF"), 0);
}

// Blink the led with a set period in ms
static int cmd_led_blink(const struct shell *sh, size_t argc, char **argv)
{
	blink_period = atoi(argv[1]); // store the argument in global variable
	if (blink_period > 0)
	{
		stop_blinking(); // stop previous work processing to react to new command exec
		shell_print(sh, "Starting to blink with period %d ms", blink_period);
		k_work_init_delayable(&led_work, led_blink_work);
		k_work_schedule(&led_work, K_NO_WAIT); // schedule the first execution to start immediately
	}
	else
	{
		shell_print(sh, "Setting blink period not successful");
		return 1;
	};
	return 0;
}

// Register commands
SHELL_CMD_REGISTER(ping, NULL, "Respond with pong", cmd_demo_ping);
SHELL_CMD_REGISTER(params, NULL, "Print params command.", cmd_demo_params);
SHELL_CMD_REGISTER(led_on, NULL, "Turn the LED ON", cmd_led_on);
SHELL_CMD_REGISTER(led_off, NULL, "Turn the LED OFF", cmd_led_off);
SHELL_CMD_ARG_REGISTER(led_blink, NULL, "Blink the LED with a defined period in ms.", cmd_led_blink, 2, NULL); // 2 mandatory args - the command name and ther period

int main(void)
{
	LOG_INF("Program starting"); // example info message
	// Init the LED device in logic 1 state
	int ret;
	if (!gpio_is_ready_dt(&led))
	{
		return 0;
	}
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		return 0;
	}
	// Shell config
	const struct device *dev;
	uint32_t dtr = 0;
	dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
	if (!device_is_ready(dev) || usb_enable(NULL))
	{
		return 0;
	}
	while (!dtr) // wait for shell device to be ready
	{
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		k_sleep(K_MSEC(100));
	}
	LOG_INF("USB device configured and connected");
	// Wait in this loop for shell commands or process messages
	while (true)
	{
		k_msleep(30); /* sleep 30 ms*/
	}

	return 0;
}
