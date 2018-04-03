/*
 * hremap -- key remapper for Linux
 * (C) IMAI Hiroei
 */
#include <stdio.h>
#include "hremap.h"

extern bool g_debug;
#define DP(x) if (g_debug) printf x

extern bool g_enable_ctrl_map;

#define BIT_LEFTCTRL   (1 << 0)
#define BIT_RIGHTCTRL  (1 << 1)
#define BIT_LEFTSHIFT  (1 << 2)
#define BIT_RIGHTSHIFT (1 << 3)
#define BIT_LEFTALT    (1 << 4)
#define BIT_RIGHTALT   (1 << 5)

#define IS_CTRL_ON()   ((m_metaKeyFlags & (BIT_LEFTCTRL|BIT_RIGHTCTRL)) != 0)
#define IS_SHIFT_ON()  ((m_metaKeyFlags & (BIT_LEFTSHIFT|BIT_RIGHTSHIFT)) != 0)
#define IS_ALT_ON()    ((m_metaKeyFlags & (BIT_LEFTALT|BIT_RIGHTALT)) != 0)

const int HreMapConverter::m_metaBits[] = {
    BIT_LEFTCTRL,  BIT_RIGHTCTRL,
    BIT_LEFTSHIFT, BIT_RIGHTSHIFT,
    BIT_LEFTALT,   BIT_RIGHTALT,
};

const __u16 HreMapConverter::m_metaKeys[] = {
    KEY_LEFTCTRL,  KEY_RIGHTCTRL,
    KEY_LEFTSHIFT, KEY_RIGHTSHIFT,
    KEY_LEFTALT,   KEY_RIGHTALT,
};

const char* HreMapConverter::m_metaKeyNames[] = {
    "L-CTRL",  "R-CTRL",
    "L-SHIFT", "R-SHIFT",
    "L-ALT",   "R-ALT",
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

HreMapConverter::HenkanKey* HreMapConverter::find_henkan_keys(int code)
{
    for (HreMapConverter::HenkanKey* p = m_henkan_keys; p->code != 0; ++p) {
        if (p->code == code)
            return p;
    }
    return NULL;
}

HreMapConverter::~HreMapConverter()
{}

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

    struct input_event syn;
    memset(&syn, 0, sizeof(syn));
    input.type = EV_KEY;
    syn.type = EV_SYN;
    syn.code = SYN_REPORT;
    syn.value = 0;
    addOutput(&syn);
}

void HreMapConverter::typeKey(__u16 code, int metaKeys)
{
    int tmpRelease = m_metaKeyFlags & ~metaKeys;
    int tmpPress = ~m_metaKeyFlags & metaKeys;
    static const int bits[] = {
        BIT_LEFTCTRL, BIT_RIGHTCTRL,
        BIT_LEFTSHIFT, BIT_RIGHTSHIFT,
        BIT_LEFTALT, BIT_RIGHTALT,
    };
    static const __u16 keys[] = {
        KEY_LEFTCTRL, KEY_RIGHTCTRL,
        KEY_LEFTSHIFT, KEY_RIGHTSHIFT,
        KEY_LEFTALT, KEY_RIGHTALT,
    };

    struct input_event input, syn;
    memset(&input, 0, sizeof(input));
    input.type = EV_KEY;
    memset(&syn, 0, sizeof(syn));
    syn.type = EV_SYN;
    syn.code = SYN_REPORT;
    syn.value = 0;

    /* Change meta-key states to metaKeys */
    input.value = 0;
    for (int i = 0; i < int(sizeof(keys)/sizeof(keys[0])); i++) {
        if (tmpRelease & bits[i]) {
            input.code = keys[i];
            addOutput(&input);
        }
    }

    input.value = 1;
    for (int i = 0; i < int(sizeof(keys)/sizeof(keys[0])); i++) {
        if (tmpPress & bits[i]) {
            input.code = keys[i];
            addOutput(&input);
        }
    }

    addOutput(&syn);

    /* Press & release the given keycode */
    input.code = code;

    input.value = 1;
    addOutput(&input);
    addOutput(&syn);

    input.value = 0;
    addOutput(&input);
    addOutput(&syn);

    /* Change meta-key states back to m_metaKeyFlags */
    input.value = 0;
    for (int i = 0; i < int(sizeof(keys)/sizeof(keys[0])); i++) {
        if (tmpPress & bits[i]) {
            input.code = keys[i];
            addOutput(&input);
        }
    }

    input.value = 1;
    for (int i = 0; i < int(sizeof(keys)/sizeof(keys[0])); i++) {
        if (tmpRelease & bits[i]) {
            input.code = keys[i];
            addOutput(&input);
        }
    }

    addOutput(&syn);
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

    struct input_event syn;
    memset(&syn, 0, sizeof(syn));
    syn.type = EV_SYN;
    syn.code = SYN_REPORT;
    syn.value = 0;
    addOutput(&syn);
}

void HreMapConverter::releaseKey(__u16 code, int metaKeys)
{
    struct input_event input;
    memset(&input, 0, sizeof(input));
    input.type = EV_KEY;
    input.code = code;
    input.value = 0;
    addOutput(&input);

    struct input_event syn;
    memset(&syn, 0, sizeof(syn));
    syn.type = EV_SYN;
    syn.code = SYN_REPORT;
    syn.value = 0;
    addOutput(&syn);

    if (metaKeys >= 0) {
        int tmpRelease = m_metaKeyFlags & ~metaKeys;
        int tmpPress = ~m_metaKeyFlags & metaKeys;
        setMetaKeys(tmpRelease, tmpPress);
    }
}

bool HreMapConverter::handleKeyInput(struct input_event* input)
{
    assert(input->type == EV_KEY);
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
                typeKey(KEY_BACKSPACE, 0); //m_metaKeyFlags & ~(BIT_RIGHTCTRL|BIT_LEFTCTRL));
                return true;
            }
            break;
        default:
            DP(("%ld\tIGNORE input->value:%d", time(NULL), input->value));
        }
	break;
    case KEY_M:
        switch (input->value) {
        case 0:
            break;
        case 2: // AUTOREPEAT
        case 1: // PRESS
            if (g_enable_ctrl_map &&
                    IS_CTRL_ON() && !IS_ALT_ON() && !IS_SHIFT_ON()) {
                DP(("C-M -> TYPE ENTER\n"));
                typeKey(KEY_ENTER, 0); //m_metaKeyFlags & ~(BIT_RIGHTCTRL|BIT_LEFTCTRL));
                return true;
            }
            break;
        default:
            DP(("%ld\tIGNORE input->value:%d", time(NULL), input->value));
        }
    break;
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
