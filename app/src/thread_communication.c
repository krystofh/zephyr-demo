#include "thread_communication.h"

//// ----------- Thread communication example -------------------------------------------------------
LOG_MODULE_REGISTER(messaging, CONFIG_LOG_DEFAULT_LEVEL); // Registers the log level for the module

// Message queue declaration using project settings
K_MSGQ_DEFINE(msg_queue, CONFIG_MSG_BYTESIZE, CONFIG_QUEUE_SIZE, 4); // msg queue aligned to 4 bytes
K_FIFO_DEFINE(fifo_queue);                                           // FIFO, There is no limit to the number of items that may be queued.
K_FIFO_DEFINE(sensor_queue);                                         // FIFO, There is no limit to the number of items that may be queued.

// Thread stack sizes
#define STACK_SIZE 1024

// Thread priorities
#define PRODUCER_PRIORITY 7
#define CONSUMER_PRIORITY 7

// Spawn thread(s) for (mockup) sensor readings
#ifdef CONFIG_SENSOR
K_THREAD_DEFINE(sensor_producer_tid, STACK_SIZE, sensor_producer_thread, NULL, NULL, NULL,
                PRODUCER_PRIORITY, 0, 0);
K_THREAD_DEFINE(sensor_consumer_tid, STACK_SIZE, sensor_consumer_thread, NULL, NULL, NULL,
                CONSUMER_PRIORITY, 0, 0);
#endif // CONFIG_SENSOR

int msg_counter = 0;    // count total number of sent FIFO messages (no reset)
int sensor_counter = 0; // count total number of values sent by a mockup sensor
const char *static_message = "static message example";
sys_slist_t linked_list;

// ----- Thread communication demo ------------------------------------------
//  Mockup sensor producing data in FIFO
void sensor_producer_thread(void)
{
    while (1)
    {
        // Allocate memory for the message from the heap
        struct sensor_data_t *sensor_msg = k_malloc(sizeof(struct sensor_data_t));
        if (sensor_msg == NULL)
        {
            LOG_ERR("Failed to allocate memory for sensor message");
            continue;
        }
        sensor_msg->msg_counter = sensor_counter;
        sensor_msg->value = 0;                 // TODO random number generator later
        k_fifo_put(&sensor_queue, sensor_msg); // send message to sensor FIFO
        LOG_INF("Sensor recorded value nr. %d", sensor_counter);
        ++sensor_counter;
        k_sleep(K_SECONDS(10));
    }
}

// Consumer thread for FIFO
void sensor_consumer_thread(void)
{
    struct sensor_data_t *sensor_rx;
    while (1)
    {
        sensor_rx = k_fifo_get(&sensor_queue, K_FOREVER); // wait until message is obtained
        LOG_INF("Sensor value nr. %d: %d", sensor_rx->msg_counter, sensor_rx->value);
        // Free the memory after processing the message
        k_free(sensor_rx);
        k_sleep(K_MSEC(100));
    }
}

// ------ COMMANDS ------------------------------------------------

int cmd_send_msgq(const struct shell *sh, size_t argc, char **argv)
{
    int ret = k_msgq_put(&msg_queue, argv[1], K_NO_WAIT); // K_NO_WAIT if sending in any case, K_FOREVER for waiting if can't be sent
    if (ret == 0)
    {
        LOG_INF("MSGQ new message sent. %d/%d in total. ", k_msgq_num_used_get(&msg_queue), CONFIG_QUEUE_SIZE);
    }
    else if ((k_msgq_num_used_get(&msg_queue)) == CONFIG_QUEUE_SIZE)
    {
        LOG_ERR("MSGQ full. Message not sent.");
    }
    else
    {
        LOG_ERR("MSGQ failed to send message");
    }
    return 0;
}

int cmd_read_msgq(const struct shell *sh, size_t argc, char **argv)
{
    char received_msg[CONFIG_MSG_BYTESIZE];                     // for storage of received chars
    int ret = k_msgq_get(&msg_queue, &received_msg, K_NO_WAIT); // alternatively K_FOREVER but here it would freeze
    if (ret == 0)
    {
        LOG_INF("MSGQ received \"%s\"\tIn queue remain: [%d]", received_msg, k_msgq_num_used_get(&msg_queue));
    }
    else if ((k_msgq_num_used_get(&msg_queue)) == 0)
    {
        LOG_INF("MSGQ empty");
    }
    else
    {
        LOG_ERR("MSGQ failed to receive message");
    }
    return 0;
}

int cmd_send_fifo(const struct shell *sh, size_t argc, char **argv)
{
    // Allocate memory for the message from the heap
    struct data_item_t *fifo_message = k_malloc(sizeof(struct data_item_t));
    if (fifo_message == NULL)
    {
        LOG_ERR("Failed to allocate memory for FIFO message");
    }
    fifo_message->msg_counter = msg_counter;
    strncpy(fifo_message->info, argv[1], sizeof(fifo_message->info) - 1);
    fifo_message->info[sizeof(fifo_message->info) - 1] = '\0'; // null termiantion
    k_fifo_put(&fifo_queue, fifo_message);                     // send message to FIFO
    LOG_INF("Message %d saved to FIFO", msg_counter);
    ++msg_counter;
    return 0;
}

int cmd_read_fifo(const struct shell *sh, size_t argc, char **argv)
{
    struct data_item_t *fifo_received;
    fifo_received = k_fifo_get(&fifo_queue, K_NO_WAIT);
    if (fifo_received == NULL)
    {
        LOG_INF("FIFO empty");
    }
    else
    {
        LOG_INF("Read FIFO msg nr. %d \"%s\"", fifo_received->msg_counter, fifo_received->info);
    }
    k_free(fifo_received);
    return 0;
}

// init linked list
void init_ll(void)
{
    sys_slist_init(&linked_list); // initialise the linked list
}

// store in linked list
int cmd_send_ll(const struct shell *sh, size_t argc, char **argv)
{
    struct list_node_t *new_node = k_malloc(sizeof(struct list_node_t));
    if (!new_node)
    {
        LOG_ERR("Linked list node memory allocation failed!");
    }
    strncpy(new_node->info, argv[1], sizeof(new_node->info) - 1);
    new_node->info[sizeof(new_node->info) - 1] = '\0'; // null termiantion
    sys_slist_append(&linked_list, &new_node->next);
    LOG_INF("Linked list node appended. %d nodes in total.", sys_slist_len(&linked_list));
    return 0;
}

// read from linked list
int cmd_read_ll(const struct shell *sh, size_t argc, char **argv)
{
    struct list_node_t *p_node = sys_slist_get(&linked_list); // Fetch and remove the first node of the given list
    if (p_node == NULL)
    {
        LOG_INF("Linked list empty");
    }
    else
    {
        LOG_INF("Read list node with message \"%s\"", p_node->info);
    }
    k_free(p_node);
    return 0;
}

// Register commands
SHELL_CMD_ARG_REGISTER(send_msgq, NULL, "Send a message to MSGQ", cmd_send_msgq, 2, NULL); // 2 mandatory args - the command name and ther message itself
SHELL_CMD_REGISTER(read_msgq, NULL, "Read first (oldest) message from MSGQ", cmd_read_msgq);

SHELL_CMD_ARG_REGISTER(send_fifo, NULL, "Send a message to FIFO message queue", cmd_send_fifo, 2, NULL); // 2 mandatory args - the command name and ther message itself
SHELL_CMD_REGISTER(read_fifo, NULL, "Read first (oldest) message from the FIFO queue", cmd_read_fifo);

SHELL_CMD_ARG_REGISTER(send_ll, NULL, "Send a message to linked list", cmd_send_ll, 2, NULL); // 2 mandatory args - the command name and ther message itself
SHELL_CMD_REGISTER(read_ll, NULL, "Read message from the linked list", cmd_read_ll);
