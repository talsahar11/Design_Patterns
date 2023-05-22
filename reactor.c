#include "reactor.h"
#include <stdlib.h>
#include <stdio.h>

// Function to insert a key-value pair into the hashmap
void insert(struct Reactor *reactor, int key, handler_t value)
{
    int index = key % reactor->pollsize;
    reactor->map->array[index].key = key;
    reactor->map->array[index].value = value;
}

// Function to retrieve the value associated with a given key
handler_t get(struct Reactor *reactor, int index)
{
//    printf("Hash: index = %d = key / poolsize = %d / %d \n", index, reactor->pollsize) ;
    return reactor->map->array[index].value;
}

// Function to delete a key-value pair from the hashmap
void removeKey(struct Reactor *reactor, int key)
{
    int index = key % reactor->pollsize;
    reactor->map->array[index].key = -1; // Assuming -1 is not a valid key
}


 // Constructor function for Reactor
void* createReactor() {
    struct Reactor* reactor = (struct Reactor*)malloc(sizeof(struct Reactor));

    if (reactor == NULL) {
        fprintf(stderr, "Memory allocation failed for Reactor!\n");
        return NULL;
    }

    reactor->pfds = (struct pollfd*)malloc(DEFAULT_POLL_SIZE * sizeof(struct pollfd));
    if (reactor->pfds == NULL) {
        fprintf(stderr, "Memory allocation failed for pfds!\n");
        free(reactor);
        return NULL;
    }

    reactor->map = (struct HashMap*)malloc(sizeof(struct HashMap));
    if (reactor->map == NULL) {
        fprintf(stderr, "Memory allocation failed for map!\n");
        free(reactor->pfds);
        free(reactor);
        return NULL;
    }

    reactor->map->array = (struct KeyValue*)malloc(DEFAULT_POLL_SIZE * sizeof(struct KeyValue));
    if (reactor->map->array == NULL) {
        fprintf(stderr, "Memory allocation failed for map->array!\n");
        free(reactor->pfds);
        free(reactor->map);
        free(reactor);
        return NULL;
    }

    reactor->fdscount = 0;
    reactor->pollsize = DEFAULT_POLL_SIZE;

    reactor->is_running = 0;
    reactor->threadfunc = NULL;

    return reactor;
}

int resize(struct Reactor* reactor) {
    int newSize = reactor->pollsize*2 ;
    // Resize pfds
    struct pollfd* newPfds = (struct pollfd*)realloc(reactor->pfds, newSize * sizeof(struct pollfd));
    if (newPfds == NULL) {
        fprintf(stderr, "Memory reallocation failed for pfds!\n");
        return 0; // Return 0 to indicate failure
    }
    reactor->pfds = newPfds;

    // Resize map
    struct KeyValue* newArray = (struct KeyValue*)realloc(reactor->map->array, newSize * sizeof(struct KeyValue));
    if (newArray == NULL) {
        fprintf(stderr, "Memory reallocation failed for map->array!\n");
        return 0; // Return 0 to indicate failure
    }
    reactor->map->array = newArray;
    reactor->pollsize = newSize ;
    printf("RESIZEEEEEEE%d\n",reactor->pollsize);
    return 1; // Return 1 to indicate success
}

void deleteFd(struct Reactor *reactor, int fd)
{
    for (int i = 0; i < reactor->fdscount; i++)
    {
        if (reactor->pfds[i].fd == fd)
        {
            reactor->pfds[i] = reactor->pfds[reactor->fdscount - 1];
            reactor->fdscount--;
            return;
        }
    }
    printf("Failed to delete fd from pfds.\n");
}

void addFd(void *this, int fd, handler_t handler)
{
    struct Reactor *reactor = (struct Reactor *)this;
    printf("Adding fd %d\n", fd);
    if (reactor->fdscount == reactor->pollsize)
    {
        resize(reactor);
    }
    
        reactor->pfds[reactor->fdscount].fd = fd;
        reactor->pfds[reactor->fdscount].events = POLLIN;
        insert(reactor, reactor->fdscount, handler);
        reactor->fdscount++;
    
}

void *threadFunc(void *this)
{
    printf("shitass\n");

    struct Reactor *reactor = (struct Reactor *)this;
    handler_t current_handler;
    while (reactor->is_running)
    {
        // printf("Thread running%d\n", reactor->fdscount) ;
        int poll_count = poll(reactor->pfds, reactor->fdscount, 0);
        //  printf("Thread running%d\n", poll_count) ;
        if (poll_count == -1)
        {
            perror("poll");
            exit(1);
        }
        //  printf("fdcount : %d", reactor->fdscount);
        for (int i = 0; i < reactor->fdscount; i++)
        {
            if (reactor->pfds[i].revents && POLLIN)
            {
                current_handler = get(reactor, i);
                printf("Received shit from ------ %p\n", current_handler) ;
                current_handler(this, reactor->pfds[i].fd);
            }
        }
    }

    return NULL;
}

void startReactor(void *this)
{
    printf("shitass\n");
    struct Reactor *reactor = (struct Reactor*)this;
    
    if (reactor->is_running != 1)
    {
    printf("shitass\n");

        reactor->is_running = 1;
    printf("shitass\n");

        int result = pthread_create(&(reactor->thread), NULL, threadFunc, reactor);
    printf("shitass\n");

        if (result != 0)
        {
            perror("Thread: ");
        }
    }
}

void stopReactor(void *this)
{
    struct Reactor *reactor = (struct Reactor *)this;
    reactor->is_running = 0;
}

void waitFor(void *this)
{
    struct Reactor *reactor = (struct Reactor *)this;
    pthread_join(reactor->thread, NULL);
}
