#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/led_control.c)
target_sources(app PRIVATE src/console_demo.c)
target_sources(app PRIVATE src/button_control.c)
target_sources(app PRIVATE src/thread_communication.c)
# target_sources(app PRIVATE src/button_demo.c)



