#include <stdint.h>

typedef enum
{
    IDLE           = 0,
    CLIENT_READING = 1,
    CLIENT_WRITING = 2,
    SERVER_READING = 3,
    SERVER_WRITING = 4,
} conn_state_t;

typedef enum
{
    SERVER,
    CLIENT,
} conn_type_t;

typedef enum
{
    BLOCKING,
    NONBLOCKING,
} blocking_behavior_t;

typedef struct conn_header
{
    conn_state_t state;
    uint64_t data_length;
    uint64_t transaction_num;
    uint64_t response_num;
    uint64_t sibling_fd;
    uint8_t data[0];
} conn_header_t;

typedef struct conn
{
    void * in_buffer;
    uint64_t in_fd;
    void * out_buffer;
    uint64_t out_fd;
    conn_type_t type;
} conn_t;

int connect(conn_t * conn, int addr, int port);
int accept(conn_t * conn, int addr, int port);

// Acquire/release connection and copy process memory in shared memory
int send_safe(conn_t conn,
              void * data,
              uint64_t data_len,
              blocking_behavior_t behavior);

// Acquire connection and allow direct access to shared memory through *ptr_to_shm
int send_unsafe_acquire(conn_t conn,
                        void ** ptr_to_shm,
                        uint64_t * max_len,
                        blocking_behavior_t behavior);

// Release a connection acquire with send_unsafe
int send_unsafe_release(conn_t conn)

// Lock connection and copies shared memory into process-owned memory
int recv_safe(conn_t conn,
              void * user_data,
              uint64_t user_length,
              blocking_behavior_t behavior);

// Acquire the connection and provide direct access shared memory through *ptr_to_shm
int recv_unsafe_acquire(conn_t conn,
                        void ** ptr_to_shm,
                        uint64_t * len,
                        blocking_behavior_t behavior);

// Release the connection acquired with recv_unsafe
int recv_unsafe_release(conn_t conn);
