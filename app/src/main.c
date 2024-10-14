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

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 500

/*
 * Ensure that an overlay for USB serial has been defined.
 */
BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
			 "Console device is not ACM CDC UART device");

LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL); // registers the log level for the module MAIN specified in Kconfig

int main(void)
{
	printk("Program starting\n"); // example info message
	// Init the LED devices in logic 1 state
	if (!init_leds())
	{
		printk("LED devices initialised properly");
	}
	else
	{
		printk("LEDs could not be initialised!");
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
	LOG_INF("USB device configured and connected\n");
	// Wait in this loop for shell commands or process messages
	while (true)
	{
		k_msleep(30); /* sleep 30 ms*/
	}

	return 0;
}
