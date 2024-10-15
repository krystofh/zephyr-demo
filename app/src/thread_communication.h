#ifndef THREAD_COMMUNICATION_H
#define THREAD_COMMUNICATION_H

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <zephyr/logging/log.h>

// SWITCH between static fifo messages and dynamically allocated
#define FIFO_DYNAMIC 0

extern int msg_counter;
extern const char *static_message;

#if FIFO_DYNAMIC
struct data_item_t
{
    void *fifo_reserved; /* 1st word reserved for use by FIFO */
    int msg_counter;
    char info[50]; // set to a fixed max. length
};
#else
extern struct data_item_t
{
    void *fifo_reserved; /* 1st word reserved for use by FIFO */
    int msg_counter;
    char *info;
} fifo_message;
#endif // FIFO_DYNAMIC

void msgq_producer_thread(void);
void msgq_consumer_thread(void);

void fifo_producer_thread(void);
void fifo_consumer_thread(void);

#endif // THREAD_COMMUNICATION_H