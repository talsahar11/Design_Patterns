#include <poll.h>
#include <pthread.h>

#define DEFAULT_POLL_SIZE 2

typedef void (*handler_t)(void*, int) ;
struct KeyValue {
    int key;
    handler_t value;
};

struct HashMap {
    struct KeyValue *array;
};

struct Reactor {
    struct pollfd* pfds ;
    struct HashMap* map ;
    pthread_t thread ;
    int fdscount ;
    int pollsize ;
    int is_running ;
    void (*threadfunc)(int*) ;
};

void* createReactor() ;
void stopReactor(void* this) ;
void startReactor(void* this) ;
void addFd(void* this, int fd, handler_t handler) ;
void waitFor(void* this) ;
void deleteFd(struct Reactor *reactor, int fd) ;