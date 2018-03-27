/*
 * hremap -- key remapper for Linux
 * (C) IMAI Hiroei
 *
 * xkeymacs substitute for X11/Linux
 * Copyright 2011 (C) Takeshi Yashiro <yashiromann@nifty.com>
 *
 * keyboard.h: keyboard device input/output
 */

#ifndef _DEFINE_H_X11KEYMACS_KEYBOARD_
#define _DEFINE_H_X11KEYMACS_KEYBOARD_

#include <string>
#include <linux/input.h>

#define USERDEV_FILENAME "/dev/uinput"

/*!
 * Keyboard device input/output
 */
class KeyboardDevice
{
public:
    KeyboardDevice(const char* filename, bool grab = true);
    ~KeyboardDevice();

    bool isKeyDevValid() const {
        return m_keyfd >= 0;
    }
    bool isUserDevValid() const {
        return m_userfd >= 0;
    }

    bool getKey(struct input_event* key);
    bool putKey(struct input_event* key);

private:
    void releaseAllKeys(int fd);

    bool m_grab;
    int m_keyfd;
    int m_userfd;
    int m_metakeys;
};

#endif /* _DEFINE_H_X11KEYMACS_KEYBOARD_ */
