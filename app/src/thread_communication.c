#include "thread_communication.h"

// Message queue declaration
#define QUEUE_SIZE 10
#define MSG_SIZE 9 // 8 chars for "statická" + 1 for null terminator

K_MSGQ_DEFINE(my_msgq, MSG_SIZE, QUEUE_SIZE, 4); // msg queue aligned to 4 bytes

// Thread stack sizes
#define STACK_SIZE 1024

// Thread priorities
#define PRODUCER_PRIORITY 1
#define CONSUMER_PRIORITY 1

K_THREAD_DEFINE(producer_tid, STACK_SIZE, producer_thread, NULL, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(consumer_tid, STACK_SIZE, consumer_thread, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);

const char *static_message = "static";

// Producer thread function sensing static message every x seconds
void producer_thread(void)
{
    while (1)
    {
        int ret = k_msgq_put(&my_msgq, static_message, K_FOREVER); // Block sending more if full
        if (ret == 0)
        {
            printk("Message sent: %s\n", static_message);
        }
        else
        {
            printk("Failed to send message\n");
        }

        k_sleep(K_SECONDS(1));
    }
}

// Consumer thread function
void consumer_thread(void)
{
    char received_msg[MSG_SIZE]; // for storage of received chars

    while (1)
    {
        // Receive message every 5 seconds
        int ret = k_msgq_get(&my_msgq, &received_msg, K_FOREVER); // Block if empty
        if (ret == 0)
        {
            printk("Message received: %s\n", received_msg);
        }
        else
        {
            printk("Failed to receive message\n");
        }

        k_sleep(K_SECONDS(5));
    }
}

// void init_threads(void)
// {
//     K_THREAD_DEFINE(producer_tid, STACK_SIZE, producer_thread, NULL, NULL, NULL,
//                     PRODUCER_PRIORITY, 0, 0);
//     K_THREAD_DEFINE(consumer_tid, STACK_SIZE, consumer_thread, NULL, NULL, NULL,
//                     CONSUMER_PRIORITY, 0, 0);
// }