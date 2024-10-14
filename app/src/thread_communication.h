#ifndef THREAD_COMMUNICATION_H
#define THREAD_COMMUNICATION_H

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>

extern const char *static_message;

void producer_thread(void);
void consumer_thread(void);
// void init_threads(void);

#endif // THREAD_COMMUNICATION_H