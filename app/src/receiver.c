#include "receiver.h"

#include <assert.h>
#include <SDL2/SDL_clipboard.h>

#include "config.h"
#include "device_msg.h"
#include "util/lock.h"
#include "util/log.h"
#include "file_handler.h"

static struct file_handler file_handler;
bool file_handler_initialized = false;

struct receiver *receiver ;

bool
receiver_init(struct receiver *receiver, socket_t control_socket, const char *serial) {

    receiver->serial = serial;

    if (!file_handler_init(&file_handler, receiver->serial, "/sdcard/")) {
        return false;
    }
    LOGI("FILE HANDLER INITIALIZED");
    file_handler_initialized = true;

    if (!(receiver->mutex = SDL_CreateMutex())) {
        return false;
    }
    receiver->control_socket = control_socket;
    return true;
}

void
receiver_destroy(struct receiver *receiver) {

    if (file_handler_initialized) {
        file_handler_stop(&file_handler);
    }
    if (file_handler_initialized) {
        file_handler_join(&file_handler);
        file_handler_destroy(&file_handler);
    }


    SDL_DestroyMutex(receiver->mutex);
}


static void
process_msg(struct device_msg *msg) {
    switch (msg->type) {
        case DEVICE_MSG_TYPE_CLIPBOARD:
            LOGI("Device clipboard copied");
            SDL_SetClipboardText(msg->clipboard.text);            
            break;
        case DEVICE_MSG_TYPE_SCREENSHOT:
            LOGI("Device screenshot %s",msg->clipboard.text);
            file_handler_action_t action;
            action = ACTION_PULL_FILE;
            file_handler_request(&file_handler, action, msg->clipboard.text);
            break;
    }
}

static ssize_t
process_msgs(const unsigned char *buf, size_t len) {
    size_t head = 0;
    for (;;) {
        struct device_msg msg;
        ssize_t r = device_msg_deserialize(&buf[head], len - head, &msg);
        if (r == -1) {
            return -1;
        }
        if (r == 0) {
            return head;
        }

        process_msg(&msg);
        device_msg_destroy(&msg);

        head += r;
        assert(head <= len);
        if (head == len) {
            return head;
        }
    }
}

static int
run_receiver(void *data) {
    receiver = data;

    unsigned char buf[DEVICE_MSG_SERIALIZED_MAX_SIZE];
    size_t head = 0;

    for (;;) {
        assert(head < DEVICE_MSG_SERIALIZED_MAX_SIZE);
        ssize_t r = net_recv(receiver->control_socket, buf,
                             DEVICE_MSG_SERIALIZED_MAX_SIZE - head);
        if (r <= 0) {
            LOGD("Receiver stopped");
            break;
        }

        ssize_t consumed = process_msgs(buf, r);
        if (consumed == -1) {
            // an error occurred
            break;
        }

        if (consumed) {
            // shift the remaining data in the buffer
            memmove(buf, &buf[consumed], r - consumed);
            head = r - consumed;
        }
    }

    return 0;
}

bool
receiver_start(struct receiver *receiver) {
    LOGD("Starting receiver thread");

    receiver->thread = SDL_CreateThread(run_receiver, "receiver", receiver);
    if (!receiver->thread) {
        LOGC("Could not start receiver thread");
        return false;
    }

    return true;
}

void
receiver_join(struct receiver *receiver) {
    SDL_WaitThread(receiver->thread, NULL);
}
