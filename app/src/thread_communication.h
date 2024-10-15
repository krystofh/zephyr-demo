#ifndef THREAD_COMMUNICATION_H
#define THREAD_COMMUNICATION_H

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <zephyr/logging/log.h>

extern int msg_counter;
extern const char *static_message;

struct data_item_t
{
    void *fifo_reserved; /* 1st word reserved for use by FIFO */
    int msg_counter;
    char *info;
};

void msgq_producer_thread(void);
void msgq_consumer_thread(void);

void fifo_producer_thread(void);
void fifo_consumer_thread(void);

#endif // THREAD_COMMUNICATION_H