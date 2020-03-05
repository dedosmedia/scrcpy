#ifndef RECEIVER_H
#define RECEIVER_H

#include <stdbool.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_thread.h>

#include "config.h"
#include "util/net.h"
#include "file_handler.h"

// receive events from the device
// managed by the controller
struct receiver {
    socket_t control_socket;
    struct file_handler *file_handler;
    SDL_Thread *thread;
    SDL_mutex *mutex;
    
};

bool
receiver_init(struct receiver *receiver, socket_t control_socket, struct file_handler *file_handler);

void
receiver_destroy(struct receiver *receiver);

bool
receiver_start(struct receiver *receiver);

// no receiver_stop(), it will automatically stop on control_socket shutdown

void
receiver_join(struct receiver *receiver);

#endif
