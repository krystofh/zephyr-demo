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

// Own code
#include "led_control.h"
#include "console_demo.h"
#include "button_control.h"
#include "thread_communication.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 20

/*
 * Ensure that an overlay for USB serial has been defined.
 */
BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
			 "Console device is not ACM CDC UART device");

LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL); // registers the log level for the module MAIN specified in Kconfig

int main(void)
{
	LOG_INF("Program starting\n"); // example info message
	// Init the LED devices in logic 1 state
	if (init_leds())
	{
		LOG_ERR("LEDs could not be initialised!");
	}
	else
	{
		LOG_INF("LED devices initialised properly");
	}

	// Button config
	if (init_button())
	{
		LOG_ERR("Button init failed!");
	}
	else
	{
		LOG_INF("Button init successful!");
	}

	// Shell config
	const struct device *dev;
	uint32_t dtr = 0;
	dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
	if (!device_is_ready(dev) || usb_enable(NULL))
	{
		return 0;
	}
	// while (!dtr) // wait for shell device to be ready
	// {
	uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
	k_sleep(K_MSEC(100));
	// }
	LOG_INF("USB device configured and connected");

	init_ll(); // init linked list

	// Wait in this loop for shell commands or process messages
	while (true)
	{
		/* If we have an LED, match its state to the button's. */
		int val = gpio_pin_get_dt(&button);

		if (val >= 0)
		{
			gpio_pin_set_dt(&button_led, val);
		}
		k_msleep(SLEEP_TIME_MS); /* sleep x ms*/
	}
	return 0;
}
