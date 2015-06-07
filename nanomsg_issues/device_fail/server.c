#include "_common.h"

#include <pthread.h>

void* server_worker_thread(void* dummy)
{
    server_worker_main();
    return NULL;
}

int main(int argc, const char * argv[])
{
    alarm(10);
    int n_threads = 40;
    int i;
    pthread_t tid[n_threads];

    for (i=0; i<n_threads; ++i)
        assert(pthread_create(&(tid[i]), NULL, &server_worker_thread, NULL) == 0);

    //for (i=0; i<n_threads; ++i)
    //    assert(pthread_join(tid[i], NULL) == 0);

    device_main();

    return 0;
}
