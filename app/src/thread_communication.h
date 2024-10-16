#ifndef THREAD_COMMUNICATION_H
#define THREAD_COMMUNICATION_H

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/slist.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/random/rand32.h> // For sys_rand32_get()
#include <string.h>

extern int msg_counter;
extern int sensor_counter;
extern const char *static_message;

struct data_item_t
{
    void *fifo_reserved; /* 1st word reserved for use by FIFO */
    int msg_counter;
    char info[50]; // set to a fixed max. length
};

struct sensor_data_t
{
    void *fifo_reserved; /* 1st word reserved for use by FIFO */
    int msg_counter;
    u_int32_t value; // mockup sensor value
};

// // Linked list and node
extern sys_slist_t linked_list; // for nodes storage
struct list_node_t
{
    struct sys_snode_t *next; // pointer to the next node
    char info[50];
};

// Functions
void sensor_producer_thread(void);
void sensor_consumer_thread(void);

int cmd_send_msgq(const struct shell *sh, size_t argc, char **argv);
int cmd_read_msgq(const struct shell *sh, size_t argc, char **argv);

int cmd_send_fifo(const struct shell *sh, size_t argc, char **argv);
int cmd_read_fifo(const struct shell *sh, size_t argc, char **argv);

int cmd_send_ll(const struct shell *sh, size_t argc, char **argv);
int cmd_read_ll(const struct shell *sh, size_t argc, char **argv);

#endif // THREAD_COMMUNICATION_H