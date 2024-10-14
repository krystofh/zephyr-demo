#include "thread_communication.h"

//// ----------- Thread communication example -------------------------------------------------------
LOG_MODULE_REGISTER(messaging, CONFIG_LOG_DEFAULT_LEVEL); // Registers the log level for the module

// Message queue declaration
#define QUEUE_SIZE 3 // store only part of messages that are sent
#define MSG_SIZE 24  // 1 for each char + 1 for null terminator

K_MSGQ_DEFINE(msg_queue, MSG_SIZE, QUEUE_SIZE, 4); // msg queue aligned to 4 bytes

// Thread stack sizes
#define STACK_SIZE 1024

// Thread priorities
#define PRODUCER_PRIORITY 1
#define CONSUMER_PRIORITY 1

// Spawn threads
K_THREAD_DEFINE(producer_tid, STACK_SIZE, producer_thread, NULL, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(consumer_tid, STACK_SIZE, consumer_thread, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);

const char *static_message = "static message example";
int msg_counter = 0; // Global message counter

// Producer thread function sensing static message every 1 second
void producer_thread(void)
{
    while (1)
    {
        int ret = k_msgq_put(&msg_queue, static_message, K_NO_WAIT); // K_NO_WAIT if sending in any case, K_FOREVER for waiting if can't be sent
        if (ret == 0)
        {
            LOG_INF("Sent \"%s\"\n", static_message);
        }
        else
        {
            LOG_ERR("Failed to send message\n");
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
        int ret = k_msgq_get(&msg_queue, &received_msg, K_FOREVER); // Block if empty
        if (ret == 0)
        {
            LOG_INF("Received \"%s\"\tIn queue remain: [%d] \n", received_msg, k_msgq_num_used_get(&msg_queue));
        }
        else
        {
            LOG_ERR("Failed to receive message\n");
        }

        k_sleep(K_SECONDS(5));
    }
}