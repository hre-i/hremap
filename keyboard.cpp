/*
 * hremap -- key remapper for Linux
 * (C) IMAI Hiroei
 *
 * xkeymacs substitute for X11/Linux
 * Copyright 2011 (C) Takeshi Yashiro <yashiromann@nifty.com>
 *
 * keyboard.cpp: keyboard device input/output
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <string.h>
#include "keyboard.h"

extern bool g_debug;
#define DP(x) if (g_debug) printf x

KeyboardDevice::KeyboardDevice(const char* filename, bool grab)
    : m_grab(grab)
{
    m_keyfd = open(filename, O_RDWR);
    if (m_keyfd >= 0) {
        releaseAllKeys(m_keyfd);
        if (m_grab)
            ioctl(m_keyfd, EVIOCGRAB, 1);
    }
    m_userfd = open(USERDEV_FILENAME, O_WRONLY);
    if (m_userfd >= 0) {
        struct uinput_user_dev device;
        memset(&device, 0, sizeof(device));
        strcpy(device.name, "x11keyemacs");
        device.id.bustype = BUS_VIRTUAL;
        device.id.vendor = 1;
        device.id.product = 1;
        device.id.version = 1;
        if (write(m_userfd, &device, sizeof(device)) < 0) {
            DP(("***"));
        }
        ioctl(m_userfd, UI_SET_EVBIT, EV_KEY);
        ioctl(m_userfd, UI_SET_EVBIT, EV_REP);
        for (int i = 0; i < 256; i++)
            ioctl(m_userfd, UI_SET_KEYBIT, i);
        ioctl(m_userfd, UI_DEV_CREATE);
    }
}

KeyboardDevice::~KeyboardDevice()
{
    if (m_userfd >= 0) {
        releaseAllKeys(m_userfd);
        ioctl(m_userfd, UI_DEV_DESTROY, NULL);
        close(m_userfd);
    }
    if (m_keyfd >= 0) {
        if (m_grab)
            ioctl(m_keyfd, EVIOCGRAB, 0);
        close(m_keyfd);
    }
}

void KeyboardDevice::releaseAllKeys(int fd)
{
    struct input_event input;
    memset(&input, 0, sizeof(input));

    input.type = EV_KEY;
    input.value = 0;
    gettimeofday(&(input.time), NULL);
    for (int i = 0; i < 256; i++) {
        input.code = i;
        if (write(fd, &input, sizeof(input)) < 0) {
            DP(("*B*"));
        }
    }

    input.type = EV_SYN;
    input.code = SYN_REPORT;
    input.value = 0;
    gettimeofday(&(input.time), NULL);
    if (write(fd, &input, sizeof(input)) < 0) {
        DP(("*C*"));
    }
}

bool KeyboardDevice::getKey(struct input_event* key)
{
    ssize_t len;
    do {
        len = read(m_keyfd, key, sizeof(*key));
    } while (len == -1 && errno == EINTR);

    DP(("%ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
                    time(NULL), key->type, key->code, key->value, (int)len,
                    key->time.tv_sec, key->time.tv_usec));

    assert(len == (ssize_t)sizeof(*key));
    return (len == (ssize_t)sizeof(*key));
}

bool KeyboardDevice::putKey(struct input_event* key)
{
    ssize_t len;
    do {
        len = write(m_userfd, key, sizeof(*key));
    } while (len == -1 && errno == EINTR);

    DP(("%ld\tout: type %d, code %3d, value %d (%d) @%ld.%ld\n",
                    time(NULL), key->type, key->code, key->value, (int)len,
                    key->time.tv_sec, key->time.tv_usec));

    assert(len == (ssize_t)sizeof(*key));
    return (len == (ssize_t)sizeof(*key));
}
