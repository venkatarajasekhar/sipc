#include <stdint.h>

#include "sipc_conn.h"

inline static int read_acquire_attempt(conn_t conn)
{
    conn_header_t * conn_header = (conn_header_t *) conn.in_buffer;
    return !__sync_bool_compare_and_swap(&conn_header->state,
                                         IDLE,
                                         READ_LOCKED);
}

inline static int read_acquire(conn_t conn)
{
    while(!read_acquire_attempt(conn));
    return 0;
}

inline static int write_acquire_attempt(conn_t conn)
{
    conn_header_t * conn_header = (conn_header_t *) conn.out_buffer;
    return !__sync_bool_compare_and_swap(&conn_header->state,
                                         IDLE,
                                         WRITE_LOCKED);
}

inline static int write_acquire(conn_t conn)
{
    while(!read_acquire_attempt(conn));
    return 0;
}

int recv_unsafe_acquire(conn_t conn,
                        void  ** buffer,
                        blocking_behavior_t behavior)
{
    conn_header_t * conn_header = (conn_header_t * ) conn.in_buffer;
    if(behavior == BLOCKING)
    {
        read_acquire(conn);
    }
    else
    {
        if(!read_acquire_attempt(conn))
        {
            return 1;
        }
    }

    *buffer = conn_header->data;
    return 0;
}

int recv_unsafe_release(conn_t conn)
{
    conn_header_t * conn_header = (conn_header_t*) conn.in_buffer;
    return !__sync_bool_compare_and_swap(&conn_header->state,
                                        READ_LOCKED,
                                        IDLE);
}

int send_unsafe_acquire(conn_t conn,
                        void  ** buffer,
                        uint64_t * max_len,
                        blocking_behavior_t behavior)
{
    conn_header_t * conn_header = (conn_header_t * ) conn.in_buffer;
    if(behavior == BLOCKING)
    {
        read_acquire(conn);
    }
    else
    {
        if(!read_acquire_attempt(conn))
        {
            return 1;
        }
    }

    *buffer = conn_header->data;
    *max_len = conn_header->max_len;
    return 0;
}

int send_unsafe_release(conn_t conn)
{
    conn_header_t * conn_header = (conn_header_t*) conn.out_buffer;
    return !__sync_bool_compare_and_swap(&conn_header->state,
                                         WRITE_LOCKED,
                                         IDLE);
}
