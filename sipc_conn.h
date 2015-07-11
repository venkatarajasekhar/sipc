#include <stdint.h>

// For now, make the assumption there
// is only one reader of a connection.
// No need for semaphore semantics.
typedef enum
{
    IDLE         = 0,
    READ_LOCKED  = 1,
    WRITE_LOCKED = 2,
} conn_state_t;

// Server and client may behave differently.
typedef enum
{
    SERVER,
    CLIENT,
} conn_type_t;

// Need I say more?
typedef enum
{
    BLOCKING,
    NONBLOCKING,
} blocking_behavior_t;

// The header at the start of our shared memory block.
// Contains the state ( a locking mechanism), the sibling fd.
// This is so a client can open a new write buffer if the server times out.
typedef struct conn_header
{
    conn_state_t state;
    uint64_t max_len;
    volatile uint8_t data[0];
} conn_header_t;

// A connection is defined as an read buffer and a write buffer
// We also know whether we're the server of client so we can behave appropriately
// A conn_t has reference semantics in regards to the data in the shared memory
typedef struct conn
{
    conn_type_t type;
    volatile void * in_buffer;
    volatile void * out_buffer;
} conn_t;

int connect(conn_t * conn, int addr, int port);
int accept(conn_t * conn, int addr, int port);

// Acquire/release connection and copy process memory in shared memory
int send_safe(conn_t conn,
              volatile void * data,
              uint64_t data_len,
              blocking_behavior_t behavior);

// Acquire connection and allow direct access to shared memory through *ptr_to_shm
int send_unsafe_acquire(conn_t conn,
                        volatile void ** ptr_to_shm,
                        uint64_t * max_len,
                        blocking_behavior_t behavior);

// Release a connection acquire with send_unsafe
int send_unsafe_release(conn_t conn);

// Lock connection and copies shared memory into process-owned memory
int recv_safe(conn_t conn,
              void * user_data,
              uint64_t user_length,
              blocking_behavior_t behavior);

// Acquire the connection and provide direct access shared memory through *ptr_to_shm
int recv_unsafe_acquire(conn_t conn,
                        volatile void  ** ptr_to_shm,
                        blocking_behavior_t behavior);

// Release the connection acquired with recv_unsafe
int recv_unsafe_release(conn_t conn);
