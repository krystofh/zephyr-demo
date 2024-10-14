#ifndef CONSOLE_DEMO_H
#define CONSOLE_DEMO_H

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

// Function declarations for the shell command handlers
int cmd_demo_ping(const struct shell *sh, size_t argc, char **argv);
int cmd_demo_params(const struct shell *sh, size_t argc, char **argv);

#endif /* CONSOLE_DEMO_H */