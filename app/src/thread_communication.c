#include "thread_communication.h"

//// ----------- Thread communication example -------------------------------------------------------
LOG_MODULE_REGISTER(messaging, CONFIG_LOG_DEFAULT_LEVEL); // Registers the log level for the module

// Message queue declaration
#define QUEUE_SIZE 3 // store only part of messages that are sent
#define MSG_SIZE 24  // 1 for each char + 1 for null terminator

K_MSGQ_DEFINE(msg_queue, MSG_SIZE, QUEUE_SIZE, 4); // msg queue aligned to 4 bytes
K_FIFO_DEFINE(fifo_queue);                         // FIFO, There is no limit to the number of items that may be queued.

// Thread stack sizes
#define STACK_SIZE 1024

// Thread priorities
#define PRODUCER_PRIORITY 1
#define CONSUMER_PRIORITY 1

// Spawn threads
K_THREAD_DEFINE(msgq_producer_tid, STACK_SIZE, msgq_producer_thread, NULL, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(msgq_consumer_tid, STACK_SIZE, msgq_consumer_thread, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);
K_THREAD_DEFINE(fifo_producer_tid, STACK_SIZE, fifo_producer_thread, NULL, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(fifo_consumer_tid, STACK_SIZE, fifo_consumer_thread, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);

int msg_counter = 0; // count number of sent FIFO msgs
const char *static_message = "static message example";
struct data_item_t fifo_message;

// Producer thread function for MSGQ sending static message every 1 second
void msgq_producer_thread(void)
{
    while (1)
    {
        int ret = k_msgq_put(&msg_queue, static_message, K_NO_WAIT); // K_NO_WAIT if sending in any case, K_FOREVER for waiting if can't be sent
        if (ret == 0)
        {
            LOG_INF("MSGQ new message sent");
        }
        else
        {
            LOG_ERR("MSGQ failed to send message\n");
        }

        k_sleep(K_SECONDS(1));
    }
}

// Consumer thread function for MSGQ
void msgq_consumer_thread(void)
{
    char received_msg[MSG_SIZE]; // for storage of received chars

    while (1)
    {
        // Receive message every 5 seconds
        int ret = k_msgq_get(&msg_queue, &received_msg, K_FOREVER); // Block if empty
        if (ret == 0)
        {
            LOG_INF("MSGQ received \"%s\"\tIn queue remain: [%d] \n", received_msg, k_msgq_num_used_get(&msg_queue));
        }
        else
        {
            LOG_ERR("MSGQ failed to receive message\n");
        }

        k_sleep(K_SECONDS(5));
    }
}

//  Producer thread for FIFO
void fifo_producer_thread(void)
{
    while (1)
    {
        fifo_message.msg_counter = msg_counter;
        fifo_message.info = "test message";
        k_fifo_put(&fifo_queue, &fifo_message); // send message to FIFO
        LOG_INF("FIFO sent msg nr. %d", msg_counter);
        ++msg_counter;
        k_sleep(K_SECONDS(3));
    }
}

// Consumer thread for FIFO
void fifo_consumer_thread(void)
{
    struct data_item_t *fifo_rx;
    while (1)
    {
        fifo_rx = k_fifo_get(&fifo_queue, K_FOREVER);
        LOG_INF("FIFO received msg nr. %d \"%s\"", fifo_rx->msg_counter, fifo_rx->info);
        k_sleep(K_MSEC(100));
    }
}