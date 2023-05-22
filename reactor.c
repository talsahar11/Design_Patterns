#include "reactor.h"
#include <stdlib.h>
#include <stdio.h>

// Function to initialize the hashmap
void initializeHashMap(struct Reactor* reactor) {
    reactor->map.array = (struct KeyValue *)malloc(DEFAULT_POLL_SIZE * sizeof(struct KeyValue));
}

// Function to insert a key-value pair into the hashmap
void insert(struct Reactor *reactor, int key, handler_t value) {
    int index = key % reactor->pollsize ;
    reactor->map.array[index].key = key;
    reactor->map.array[index].value = value;
}

// Function to retrieve the value associated with a given key
handler_t get(struct Reactor *reactor, int key) {
    int index = key % reactor->pollsize ;
    return reactor->map.array[index].value;
}


// Function to delete a key-value pair from the hashmap
void removeKey(struct Reactor *reactor, int key) {
    int index = key % reactor->pollsize;
    reactor->map.array[index].key = -1; // Assuming -1 is not a valid key
}

void* createReactor(){
    struct Reactor *reactor = malloc(sizeof(struct Reactor)) ;
    reactor->pfds = malloc(sizeof(struct pollfd) * DEFAULT_POLL_SIZE);
    initializeHashMap(reactor) ;
}

void resize(struct Reactor *reactor){
    reactor->pollsize *= 2 ;
    reactor->pfds = realloc(reactor->pfds, reactor->pollsize * sizeof(struct pollfd)) ;
    reactor->map.array = realloc(reactor->map.array, reactor->pollsize * sizeof(struct KeyValue)) ;
}

void deleteFd(struct Reactor *reactor, int fd){
    for(int i = 0 ; i < reactor->fdscount ; i++){
        if(reactor->pfds[i].fd == fd){
            reactor->pfds[i] = reactor->pfds[reactor->fdscount-1] ;
            reactor->fdscount-- ;
            return ;
        }
    }
    printf("Failed to delete fd from pfds.\n") ;
}

void addFd(void* this, int fd, handler_t handler){
    struct Reactor *reactor = (struct Reactor*)this ;
    if(reactor->fdscount == reactor->pollsize){
        resize(reactor) ;
    }else{
        reactor->pfds[reactor->fdscount].fd = fd ;
        reactor->pfds[reactor->fdscount].events = POLLIN ;
        insert(reactor, fd, handler) ;
        reactor->fdscount++ ;
    }
}

void* threadFunc(void* this){
    struct Reactor *reactor = (struct Reactor*)this ;
    handler_t current_handler ;
    while(reactor->is_running){
        for(int i = 0 ; i < reactor->fdscount ; i++){
            if(reactor->pfds[i].revents && POLLIN){
                current_handler = get(reactor, reactor->pfds[i].fd);
                if(reactor->pfds[i].fd == reactor->listenerfd){
                    int* newFD = current_handler(&(reactor->listenerfd)) ;
                    addFd(this, newFD, )
                }else {
                    current_handler(NULL);
                }
            }
        }
    }
    return NULL ;
}


void startReactor(void* this){
    struct Reactor *reactor = (struct Reactor*)this ;
    if(reactor->is_running != 1) {
        reactor->is_running = 1;
        int result = pthread_create(&(reactor->thread), NULL, threadFunc, reactor) ;
        if (result != 0) {
            perror("Thread: ") ;
        }
    }
}

void stopReactor(void* this){
    struct Reactor *reactor = (struct Reactor*)this ;
    reactor->is_running = 0 ;
}

void waitFor(void* this){
    struct Reactor *reactor = (struct Reactor*)this ;
    pthread_join(reactor->thread, NULL) ;
}

