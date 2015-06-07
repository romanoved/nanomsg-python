#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>


static size_t msg_size = 5001;
static size_t max_msg_size = 10240;
static const char* prefix = "reply_to: ";
static size_t prefix_size = 10; //strlen(prefix)

static const char* server_addr  = "ipc://./ipc_server";
#define device_addr_inproc "inproc://device";
#define device_addr_ipc "ipc://ipc_device";
#define device_addr_tcp "tcp://127.0.0.1:8719";
static const char* device_addr = device_addr_ipc;


void client_main(void)
{
    size_t i = 0;
    struct nn_pollfd poll_s;
    int poll_rv = 0;

    char req_buf[msg_size];
    char rep_buf[msg_size + prefix_size];

    int sock = nn_socket (AF_SP, NN_REQ);
    assert (sock >= 0);

    poll_s.fd = sock;
    poll_s.events = NN_POLLIN;
    poll_s.revents = 0;

    assert (nn_connect (sock, server_addr) >=0 );
    usleep(1000000*1);

    size_t received = 0;
    size_t timeout = 0;

    for (;;)
    {
        for (i=0; i<sizeof(req_buf); ++i)
            req_buf[i] = 'x';//rand ();

        assert(nn_send(sock, req_buf, sizeof(req_buf), NN_DONTWAIT) == sizeof(req_buf));

        assert((poll_rv = nn_poll(&poll_s, 1, 5)) >= 0);

        if (poll_rv)
        {
            int rv = nn_recv(sock, rep_buf, sizeof(rep_buf), NN_DONTWAIT);
            if (rv != sizeof(rep_buf))
            {
                printf("nn_recv(sock, rep_buf, sizeof(rep_buf), NN_DONTWAIT) != sizeof(rep_buf): %d != %lu\n", rv, sizeof(rep_buf));
                abort();
            }
            assert(!strncmp(prefix, rep_buf, prefix_size));
            assert(!strncmp(req_buf, rep_buf+prefix_size, msg_size));
            ++received;
        }
        else
        {
            ++timeout;
        }
        if (received + timeout == 1000)
        {
            printf("received: %lu\ttimeout: %lu\n", received, timeout);
            received = timeout = 0;
        }
    }

}

void device_main(void)
{

    int sock_server = nn_socket (AF_SP_RAW, NN_REP);
    assert (sock_server >= 0);

    assert (nn_bind (sock_server, server_addr) >=0 );

    int sock_device = nn_socket (AF_SP_RAW, NN_REQ);
    assert (sock_device >= 0);

    assert (nn_bind (sock_device, device_addr) >=0 );
    usleep(1000000*1);

    nn_device (sock_server, sock_device);
}


void server_worker_main()
{
    int sock = nn_socket (AF_SP, NN_REP);
    assert (sock >= 0);

    assert (nn_connect (sock, device_addr) >=0 );
    usleep(1000000*1);

    char msg_buf[prefix_size + max_msg_size];

    memmove(msg_buf, prefix, prefix_size);

    char* msg = msg_buf + prefix_size;
    for (;;)
    {
        int recv_r = nn_recv(sock, msg, max_msg_size, 0);
        //printf("%s\n", msg);
        assert (recv_r >= 0);
        usleep((int)(1000.*(1 + 8 * rand() / RAND_MAX)));
        assert(nn_send(sock, msg_buf, prefix_size + recv_r, 0) == prefix_size + recv_r);
    }
}
