#include <poll.h>
#include <pthread.h>

#define DEFAULT_POLL_SIZE 10

typedef void (*handler_t)() ;
struct KeyValue {
    int key;
    handler_t value;
};

struct HashMap {
    struct KeyValue *array;
    int size;
};

struct Reactor {
    struct pollfd* pfds ;
    struct HashMap map ;
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