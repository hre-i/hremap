/*
 * hremap -- key remapper for Linux
 * (C) IMAI Hiroei
 */
#include <stdio.h>
#include "hremap.h"

extern bool g_debug;
#define DP(x) if (g_debug) printf x

extern bool g_enable_ctrl_map;
extern bool g_enable_function_map;
extern bool g_enable_muhenkan_map;

#define BIT_LEFTCTRL   (1 << 0)
#define BIT_RIGHTCTRL  (1 << 1)
#define BIT_LEFTSHIFT  (1 << 2)
#define BIT_RIGHTSHIFT (1 << 3)
#define BIT_LEFTALT    (1 << 4)
#define BIT_RIGHTALT   (1 << 5)
#define BIT_LEFTMETA   (1 << 6)
#define BIT_RIGHTMETA  (1 << 7)

#define IS_CTRL_ON()   ((m_metaKeyFlags & (BIT_LEFTCTRL|BIT_RIGHTCTRL)) != 0)
#define IS_SHIFT_ON()  ((m_metaKeyFlags & (BIT_LEFTSHIFT|BIT_RIGHTSHIFT)) != 0)
#define IS_ALT_ON()    ((m_metaKeyFlags & (BIT_LEFTALT|BIT_RIGHTALT)) != 0)
#define IS_WIN_ON()    ((m_metaKeyFlags & (BIT_LEFTMETA|BIT_RIGHTMETA)) != 0)
#define IS_RMETA_ONLY() (m_metaKeyFlags == BIT_RIGHTMETA)
#define IS_ALL_OFF()    (m_metaKeyFlags == 0)

const int HreMapConverter::m_metaBits[] = {
    BIT_LEFTCTRL,  BIT_RIGHTCTRL,
    BIT_LEFTSHIFT, BIT_RIGHTSHIFT,
    BIT_LEFTALT,   BIT_RIGHTALT,
    BIT_LEFTMETA,  BIT_RIGHTMETA,
};

const __u16 HreMapConverter::m_metaKeys[] = {
    KEY_LEFTCTRL,  KEY_RIGHTCTRL,
    KEY_LEFTSHIFT, KEY_RIGHTSHIFT,
    KEY_LEFTALT,   KEY_RIGHTALT,
    KEY_LEFTMETA,  KEY_RIGHTMETA,
};

const char* HreMapConverter::m_metaKeyNames[] = {
    "L-CTRL",  "R-CTRL",
    "L-SHIFT", "R-SHIFT",
    "L-ALT",   "R-ALT",
    "L-WIN",   "R-WIN",
};

#define metaKeysNum (int(sizeof(m_metaKeys)/sizeof(m_metaKeys[0])))

HreMapConverter::HenkanKey HreMapConverter::m_henkan_keys[] = {
    { KEY_H,     KEY_LEFT,      false },
    { KEY_J,     KEY_DOWN,      false },
    { KEY_K,     KEY_UP,        false },
    { KEY_L,     KEY_RIGHT,     false },
    { KEY_COMMA, KEY_HOME,      false },
    { KEY_DOT,   KEY_END,       false },
    { KEY_O,     KEY_PAGEDOWN,  false },
    { KEY_I,     KEY_PAGEUP,    false },
    { KEY_M,     KEY_DELETE,    false },
    { KEY_U,     KEY_COMPOSE,   false },
    { KEY_1,     KEY_F1,        false },
    { KEY_2,     KEY_F2,        false },
    { KEY_3,     KEY_F3,        false },
    { KEY_4,     KEY_F4,        false },
    { KEY_5,     KEY_F5,        false },
    { KEY_6,     KEY_F6,        false },
    { KEY_7,     KEY_F7,        false },
    { KEY_8,     KEY_F8,        false },
    { KEY_9,     KEY_F9,        false },
    { KEY_0,     KEY_F10,       false },
    { KEY_MINUS, KEY_F11,       false },
    { KEY_EQUAL, KEY_F12,       false },
    { -1, 0, 0 }
};

HreMapConverter::HreMapConverter()
{
    memset(&syn, 0, sizeof(syn));
    syn.type = EV_SYN;
    syn.code = SYN_REPORT;
    syn.value = 0;

    memset(&sleep, 0, sizeof(sleep));
    sleep.type = LOCAL_EV_SLEEP;
    sleep.code = 0;
    sleep.value = 0;
}

HreMapConverter::~HreMapConverter()
{}

HreMapConverter::HenkanKey* HreMapConverter::find_henkan_keys(int code)
{
    for (HreMapConverter::HenkanKey* p = m_henkan_keys; p->code != 0; ++p) {
        if (p->code == code)
            return p;
    }
    return NULL;
}

bool HreMapConverter::handleMetaKeyInput(struct input_event* input, int bit)
{
    if (input->value == 1)
        m_metaKeyFlags |= bit;
    else if (input->value == 0)
        m_metaKeyFlags &= ~bit;
    addOutput(input);
    return true;
}

void HreMapConverter::setMetaKeys(int tmpRelease, int tmpPress)
{
    /* Change meta-key states to metaKeys */
    struct input_event input;
    memset(&input, 0, sizeof(input));
    input.type = EV_KEY;
    input.value = 0;
    for (int i = 0; i < metaKeysNum; i++) {
        if (tmpRelease & m_metaBits[i]) {
            input.code = m_metaKeys[i];
            addOutput(&input);
            DP(("RELEASE %s\n", m_metaKeyNames[i]));
        }
    }

    input.value = 1;
    for (int i = 0; i < metaKeysNum; i++) {
        if (tmpPress & m_metaBits[i]) {
            input.code = m_metaKeys[i];
            addOutput(&input);
            DP(("PRESS %s\n", m_metaKeyNames[i]));
        }
    }

    addSyn();
}

void HreMapConverter::typeKey(__u16 code, int metaKeys)
{
    struct input_event input;
    memset(&input, 0, sizeof(input));
    input.type = EV_KEY;

    int tmpRelease = m_metaKeyFlags & ~metaKeys;
    int tmpPress = ~m_metaKeyFlags & metaKeys;

    /* Change meta-key states to metaKeys */
    if (metaKeys >= 0) {
        input.value = 0;
        for (int i = 0; i < metaKeysNum; i++) {
            if (tmpRelease & m_metaBits[i]) {
                input.code = m_metaKeys[i];
                addOutput(&input);
            }
        }
        input.value = 1;
        for (int i = 0; i < metaKeysNum; i++) {
            if (tmpPress & m_metaBits[i]) {
                input.code = m_metaKeys[i];
                addOutput(&input);
            }
        }
        addSyn();
    }

    /* Press & release the given keycode */
    input.code = code;

    input.value = 1;
    addOutput(&input);
    addSyn();

    input.value = 0;
    addOutput(&input);
    addSyn();

    if (metaKeys >= 0) {
        /* Change meta-key states back to m_metaKeyFlags */
        input.value = 0;
        for (int i = 0; i < metaKeysNum; i++) {
            if (tmpPress & m_metaBits[i]) {
                input.code = m_metaKeys[i];
                addOutput(&input);
            }
        }

        input.value = 1;
        for (int i = 0; i < metaKeysNum; i++) {
            if (tmpRelease & m_metaBits[i]) {
                input.code = m_metaKeys[i];
                addOutput(&input);
            }
        }
        addSyn();
    }
}

void HreMapConverter::pressKey(__u16 code, int metaKeys)
{
    if (metaKeys >= 0) {
        int tmpRelease = m_metaKeyFlags & ~metaKeys;
        int tmpPress = ~m_metaKeyFlags & metaKeys;
        setMetaKeys(tmpRelease, tmpPress);
    }

    struct input_event input;
    memset(&input, 0, sizeof(input));
    input.type = EV_KEY;
    input.code = code;
    input.value = 1;
    addOutput(&input);
    addSyn();
}

void HreMapConverter::releaseKey(__u16 code, int metaKeys)
{
    struct input_event input;
    memset(&input, 0, sizeof(input));
    input.type = EV_KEY;
    input.code = code;
    input.value = 0;
    addOutput(&input);
    addSyn();

    if (metaKeys >= 0) {
        int tmpRelease = m_metaKeyFlags & ~metaKeys;
        int tmpPress = ~m_metaKeyFlags & metaKeys;
        setMetaKeys(tmpRelease, tmpPress);
    }
}

bool HreMapConverter::handleKeyInput(struct input_event* input)
{
#define WIN_MOD (BIT_LEFTMETA|BIT_LEFTALT|BIT_LEFTCTRL|BIT_LEFTSHIFT)
    assert(input->type == EV_KEY);
    DP(("m_metaKeyFlags = %d\n", m_metaKeyFlags));
    if (g_enable_function_map && IS_ALL_OFF()) {
        switch (input->code) {
        case KEY_F1:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F* -> RightCtrl,Win+Alt+Ctrl+Shift+*\n"));
                typeKey(KEY_RIGHTCTRL, -1);
                addSleep();
                typeKey(KEY_1, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F2:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F2 -> RightCtrl,Win+Alt+Ctrl+Shift+2\n"));
                typeKey(KEY_RIGHTCTRL, -1);
                addSleep();
                typeKey(KEY_2, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F3:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F3 -> RightCtrl,Win+Alt+Ctrl+Shift+3\n"));
                typeKey(KEY_RIGHTCTRL, -1);
                addSleep();
                typeKey(KEY_3, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F4:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F4 -> RightCtrl,Win+Alt+Ctrl+Shift+4\n"));
                typeKey(KEY_RIGHTCTRL, -1);
                addSleep();
                typeKey(KEY_4, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F5:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F5 -> Win+Alt+Ctrl+Shift+H\n"));
                typeKey(KEY_H, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F6:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F6 -> Win+Alt+Ctrl+Shift+J\n"));
                typeKey(KEY_J, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F7:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F7 -> Win+Alt+Ctrl+Shift+K\n"));
                typeKey(KEY_K, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F8:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F8 -> Win+Alt+Ctrl+Shift+L\n"));
                typeKey(KEY_L, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F9:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F9 -> Win+Alt+Ctrl+Shift+I\n"));
                typeKey(KEY_Y, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F10:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F10 -> Win+Alt+Ctrl+Shift+O\n"));
                typeKey(KEY_U, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F11:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F11 -> Win+Alt+Ctrl+Shift+I\n"));
                typeKey(KEY_I, WIN_MOD);
                return true;
            }
	    break;

        case KEY_F12:
            switch (input->value) {
            case 2:
            case 1:
                DP(("F12 -> Win+Alt+Ctrl+Shift+O\n"));
                typeKey(KEY_O, WIN_MOD);
                return true;
            }
	    break;
	}
    }
    if (g_enable_muhenkan_map && m_muhenkan_state) {
        switch (input->code) {
        case KEY_1: case KEY_2: case KEY_3: case KEY_4:
            switch (input->value) {
            case 2:
            case 1:
                DP(("MUHEN+* -> RightCtrl,Win+Alt+Ctrl+Shift+*\n"));
                typeKey(KEY_RIGHTCTRL, -1);
                addSleep();
                typeKey(input->code, WIN_MOD);
                return true;
            }
	    break;

        case KEY_H: case KEY_J: case KEY_K: case KEY_L:
        case KEY_Y: case KEY_U: case KEY_I: case KEY_O:
        case KEY_Q: case KEY_W: case KEY_A: case KEY_S:
        case KEY_Z: case KEY_X: case KEY_C: case KEY_V:
            switch (input->value) {
            case 2:
            case 1:
                DP(("MUHEN+* -> Win+Alt+Ctrl+Shift+*\n"));
                typeKey(input->code, WIN_MOD);
                return true;
            }
	    break;

        case KEY_Z:
            switch (input->value) {
            case 2:
            case 1:
                DP(("MUHEN+Z -> Win+Alt+Ctrl+Shift+Z\n"));
                typeKey(KEY_Z, WIN_MOD);
                return true;
            }
	    break;
        }
    }
    switch (input->code) {
    case KEY_LEFTCTRL:
        return handleMetaKeyInput(input, BIT_LEFTCTRL);
    case KEY_RIGHTCTRL:
        return handleMetaKeyInput(input, BIT_RIGHTCTRL);
    case KEY_LEFTSHIFT:
        return handleMetaKeyInput(input, BIT_LEFTSHIFT);
    case KEY_RIGHTSHIFT:
        return handleMetaKeyInput(input, BIT_RIGHTSHIFT);
    case KEY_LEFTALT:
        return handleMetaKeyInput(input, BIT_LEFTALT);
    case KEY_RIGHTALT:
        return handleMetaKeyInput(input, BIT_RIGHTALT);
    case KEY_LEFTMETA:
        return handleMetaKeyInput(input, BIT_LEFTMETA);
    case KEY_RIGHTMETA:
        return handleMetaKeyInput(input, BIT_RIGHTMETA);
    case KEY_MUHENKAN:
        if (input->value == 0) {
            m_muhenkan_state = false;
        } else {
            m_muhenkan_state = true;
        }
        return true;
    case KEY_HENKAN:
        if (input->value == 0) {
            // Henkan がリリースされたら，同時押しの press 状態を解除する
            for (HenkanKey* p = m_henkan_keys; p->code >= 0; ++p) {
                if (p->pressed) {
                    releaseKey(p->mapped, -1);
                    p->pressed = false;
                }
            }
            // TODO: 同時押しされている(本来の)キーをプレス状態にする必要があるか？
            m_henkan_state = false;
        } else {
            m_henkan_state = true;
        }
        return true;
    }

    if (m_henkan_state) {
        for (HenkanKey* p = m_henkan_keys; p->code >= 0; ++p) {
            if (p->code == input->code) {
                if (input->value == 0) {
                    DP(("HENKAN PRESS OFF:(%d -> %d)\n", p->code, p->mapped));
                    releaseKey(p->mapped, -1);
                    p->pressed = false;
                    return true;
                } else if (input->value > 0) {
                    DP(("HENKAN PRESS ON:(%d -> %d)\n", p->code, p->mapped));
                    pressKey(p->mapped, -1);
                    p->pressed = true;
                    return true;
                }
            }
        }
    }

    switch (input->code) {
    case KEY_H:
        switch (input->value) {
        case 0:
            break;
        case 2: // AUTOREPEAT
        case 1: // PRESS
            if (g_enable_ctrl_map &&
                    IS_CTRL_ON() && !IS_ALT_ON() && !IS_SHIFT_ON()) {
                DP(("C-H -> TYPE BACKSPACE\n"));
                typeKey(KEY_BACKSPACE, 0);
                return true;
            }
            break;
        default:
            DP(("%ld\tIGNORE input->value:%d", time(NULL), input->value));
        }
        break;
    case KEY_PAUSE:
        switch (input->value) {
        case 0:
            break;
        case 2: // AUTOREPEAT
        case 1: // PRESS
            if (g_enable_ctrl_map) {
                DP(("PAUSE -> C-H\n"));
                typeKey(KEY_H, BIT_LEFTCTRL);
                return true;
            }
            break;
        default:
            DP(("%ld\tIGNORE input->value:%d", time(NULL), input->value));
        }
        break;
#if 0
    case KEY_M:
        switch (input->value) {
        case 0:
            break;
        case 2: // AUTOREPEAT
        case 1: // PRESS
            if (g_enable_ctrl_map &&
                    IS_CTRL_ON() && !IS_ALT_ON() && !IS_SHIFT_ON()) {
                DP(("C-M -> TYPE ENTER\n"));
                typeKey(KEY_ENTER, 0);
                return true;
            }
            break;
        default:
            DP(("%ld\tIGNORE input->value:%d", time(NULL), input->value));
        }
        break;
#endif
    }

    handleKeyDefault(input);
    return true;
}

bool HreMapConverter::handleInput(struct input_event* input)
{
    switch (input->type) {
    case EV_KEY:
        /* Key events are manipulated */
        return handleKeyInput(input);
    default:
        /* For other events, simply send the event as it is */
        addOutput(input);
        return true;
    }
}

bool HreMapConverter::handleKeyDefault(struct input_event* input)
{
    addOutput(input);
    return true;
}
