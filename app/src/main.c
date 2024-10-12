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
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// Example shell command handler
// Prints "pong" as a response to "ping" request
static int cmd_demo_ping(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "pong");

	return 0;
}

static int cmd_led_on(const struct shell *sh, size_t argc, char **argv)
{
	// Turn ON LED
	return (gpio_pin_set_dt(&led, 1) < 0) ? 1 : (shell_print(sh, "LED on"), 0);
}

static int cmd_led_off(const struct shell *sh, size_t argc, char **argv)
{
	// Turn OFF LED
	return (gpio_pin_set_dt(&led, 0) < 0) ? 1 : (shell_print(sh, "LED OFF"), 0);
}

// Register commands
SHELL_CMD_REGISTER(ping, NULL, "Respond with pong", cmd_demo_ping);
SHELL_CMD_REGISTER(led_on, NULL, "Turn the LED ON", cmd_led_on);
SHELL_CMD_REGISTER(led_off, NULL, "Turn the LED OFF", cmd_led_off);

int main(void)
{
	LOG_INF("Program starting"); // example info message
	int ret;
	if (!gpio_is_ready_dt(&led))
	{
		return 0;
	}
	// Turn ON LED
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

	while (!dtr)
	{
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		k_sleep(K_MSEC(100));
	}

	LOG_INF("USB device configured and connected");
	// this loop is needed to define how often log messages are processed
	while (true)
	{
		k_msleep(30); /* sleep 30 ms*/
	}

	// Turn OFF LED at the end
	ret = gpio_pin_set_dt(&led, 0);
	if (ret < 0)
	{
		return 0;
	}

	return 0;
}
