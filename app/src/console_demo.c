#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include "console_demo.h"

LOG_MODULE_REGISTER(console_demo, CONFIG_LOG_DEFAULT_LEVEL); // Registers the log level for the console_demo module

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

// Register shell commands
SHELL_CMD_REGISTER(ping, NULL, "Respond with pong", cmd_demo_ping);
SHELL_CMD_REGISTER(params, NULL, "Print params command.", cmd_demo_params);