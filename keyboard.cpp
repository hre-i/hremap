/*
 * hremap -- key remapper for Linux
 * (C) IMAI Hiroei
 *
 * xkeymacs substitute for X11/Linux
 * Copyright 2011 (C) Takeshi Yashiro <yashiromann@nifty.com>
 *
 * keyboard.cpp: keyboard device input/output
 */

#include "keyboard.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern bool g_debug;
#define DP(x)                                                                  \
  if (g_debug)                                                                 \
  printf x

KeyboardDevice::KeyboardDevice(const char *filename, bool grab) : m_grab(grab) {
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
    strcpy(device.name, "hremap");
    device.id.bustype = BUS_VIRTUAL;
    device.id.vendor = 1;
    device.id.product = 1;
    device.id.version = 1;
    write(m_userfd, &device, sizeof(device));
    ioctl(m_userfd, UI_SET_EVBIT, EV_KEY);
    ioctl(m_userfd, UI_SET_EVBIT, EV_REP);
    for (int i = 0; i < 256; i++)
      ioctl(m_userfd, UI_SET_KEYBIT, i);
    ioctl(m_userfd, UI_DEV_CREATE);
  }
}

KeyboardDevice::~KeyboardDevice() {
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

void KeyboardDevice::releaseAllKeys(int fd) {
  struct input_event input;
  memset(&input, 0, sizeof(input));

  input.type = EV_KEY;
  input.value = 0;
  gettimeofday(&(input.time), NULL);
  for (int i = 0; i < 256; i++) {
    input.code = i;
    write(fd, &input, sizeof(input));
  }

  input.type = EV_SYN;
  input.code = SYN_REPORT;
  input.value = 0;
  gettimeofday(&(input.time), NULL);
  write(fd, &input, sizeof(input));
}

bool KeyboardDevice::getKey(struct input_event *key) {
  extern bool g_jp_to_us;
  extern bool g_hhk;
  ssize_t len;
  do {
    len = read(m_keyfd, key, sizeof(*key));
  } while (len == -1 && errno == EINTR);

  DP(("%ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n", time(NULL),
      key->type, key->code, key->value, (int)len, key->time.tv_sec,
      key->time.tv_usec));

  if (g_hhk) {
    switch (key->code) {
    case KEY_GRAVE:
      key->code = KEY_ESC;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    case KEY_ESC:
      key->code = KEY_GRAVE;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    }
  }

  if (g_jp_to_us) {
    switch (key->code) {
    case KEY_SCROLLLOCK:
      key->code = KEY_CAPSLOCK;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    case KEY_RO:
      key->code = KEY_RIGHTSHIFT;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    case KEY_RIGHTSHIFT:
      key->code = KEY_RIGHTCTRL;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    case KEY_CAPSLOCK:
      key->code = KEY_LEFTCTRL;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    case KEY_KATAKANAHIRAGANA:
      key->code = KEY_RIGHTMETA;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    case KEY_LEFTCTRL:
      key->code = KEY_KATAKANA;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    case KEY_YEN:
      key->code = KEY_ESC;
      DP(("-> %ld\tin : type %d, code %3d, value %d (%d) @%ld.%ld\n",
          time(NULL), key->type, key->code, key->value, (int)len,
          key->time.tv_sec, key->time.tv_usec));
      break;
    }
  }

  assert(len == (ssize_t)sizeof(*key));

  return (len == (ssize_t)sizeof(*key));
}

bool KeyboardDevice::putKey(struct input_event *key) {
  ssize_t len;
  do {
    gettimeofday(&(key->time), NULL);
    len = write(m_userfd, key, sizeof(*key));
  } while (len == -1 && errno == EINTR);

  DP(("%ld\tout: type %d, code %3d, value %d (%d) @%ld.%ld\n", time(NULL),
      key->type, key->code, key->value, (int)len, key->time.tv_sec,
      key->time.tv_usec));

  assert(len == (ssize_t)sizeof(*key));
  return (len == (ssize_t)sizeof(*key));
}
