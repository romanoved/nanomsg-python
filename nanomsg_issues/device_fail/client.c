#include "_common.h"
#include <pthread.h>

void* client_thread(void* dummy)
{
    client_main();
    return NULL;
}

int main(void)
{
    alarm(25);
    int i, n_threads = 1;
    pthread_t tid[n_threads];

    for (i=0; i<n_threads; ++i)
        assert(pthread_create(&(tid[i]), NULL, &client_thread, NULL) == 0);
    for (i=0; i<n_threads; ++i)
        assert(pthread_join(tid[i], NULL) == 0);

    return 0;
}
