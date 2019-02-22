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
extern bool g_enable_henkan_map;
extern bool g_enable_muhenkan_map;
extern bool g_enable_katakana_map;
extern bool g_henkan_only_to_henkan;

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

#define NO_CHG (-1)

    { KEY_1,     KEY_F1,        NO_CHG,        false },
    { KEY_2,     KEY_F2,        NO_CHG,        false },
    { KEY_3,     KEY_F3,        NO_CHG,        false },
    { KEY_4,     KEY_F4,        NO_CHG,        false },
    { KEY_5,     KEY_F5,        NO_CHG,        false },
    { KEY_6,     KEY_F6,        NO_CHG,        false },
    { KEY_7,     KEY_F7,        NO_CHG,        false },
    { KEY_8,     KEY_F8,        NO_CHG,        false },
    { KEY_9,     KEY_F9,        NO_CHG,        false },
    { KEY_0,     KEY_F10,       NO_CHG,        false },
    { KEY_MINUS, KEY_F11,       NO_CHG,        false },
    { KEY_EQUAL, KEY_F12,       NO_CHG,        false },

//  { KEY_Q,                                         },
    { KEY_W,     KEY_2,         BIT_RIGHTMETA, false },
    { KEY_E,     KEY_4,         BIT_RIGHTMETA, false },
    { KEY_R,     KEY_6,         BIT_RIGHTMETA, false },
    { KEY_T,     KEY_8,         BIT_RIGHTMETA, false },
    { KEY_Y,     KEY_2,         BIT_RIGHTMETA, false },
    { KEY_U,     KEY_COMPOSE,   NO_CHG,        false },
    { KEY_I,     KEY_PAGEUP,    NO_CHG,        false },
    { KEY_O,     KEY_PAGEDOWN,  NO_CHG,        false },
//  { KEY_P,                                         },

    { KEY_A,     KEY_1,         BIT_RIGHTMETA, false },
    { KEY_S,     KEY_3,         BIT_RIGHTMETA, false },
    { KEY_D,     KEY_5,         BIT_RIGHTMETA, false },
    { KEY_F,     KEY_7,         BIT_RIGHTMETA, false },
    { KEY_G,     KEY_9,         BIT_RIGHTMETA, false },
    { KEY_H,     KEY_LEFT,      NO_CHG,        false },
    { KEY_J,     KEY_DOWN,      NO_CHG,        false },
    { KEY_K,     KEY_UP,        NO_CHG,        false },
    { KEY_L,     KEY_RIGHT,     NO_CHG,        false },

//  { KEY_Z,                                         },
//  { KEY_X,                                         },
//  { KEY_C,                                         },
//  { KEY_V,                                         },
//  { KEY_B,                                         },
    { KEY_N,     KEY_INSERT,    NO_CHG,        false },
    { KEY_M,     KEY_DELETE,    NO_CHG,        false },
    { KEY_COMMA, KEY_HOME,      NO_CHG,        false },
    { KEY_DOT,   KEY_END,       NO_CHG,        false },
    { KEY_SPACE, KEY_ESC,       NO_CHG,        false },
    { -1, 0, -1, 0 }
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
        setMetaKeys(tmpPress, tmpRelease);
    }
}

bool HreMapConverter::handleKeyInput(struct input_event* input)
{
#define WIN_MOD (BIT_LEFTMETA|BIT_LEFTCTRL|BIT_LEFTSHIFT)
    assert(input->type == EV_KEY);
    DP(("m_metaKeyFlags = %d\n", m_metaKeyFlags));

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

    case KEY_KATAKANA:
        if (g_enable_katakana_map) {
          if (input->value == 0) {
              releaseKey(KEY_LEFTCTRL,  -1);
              //releaseKey(KEY_LEFTALT,   -1);
              releaseKey(KEY_LEFTSHIFT, -1);
              releaseKey(KEY_LEFTMETA,  -1);
          } else {
              pressKey(KEY_LEFTMETA,  -1);
              pressKey(KEY_LEFTSHIFT, -1);
              //pressKey(KEY_LEFTALT,   -1);
              pressKey(KEY_LEFTCTRL,  -1);
          }
          return true;
        }
    }

    if (g_enable_henkan_map) {
        if (input->code == KEY_HENKAN) {
            if (input->value == 0) {
                if (m_henkan_only) {
                    typeKey(KEY_HENKAN, -1);
                } else {
                    // Henkan がリリースされたら，同時押しの press 状態を解除する
                    for (HenkanKey* p = m_henkan_keys; p->code >= 0; ++p) {
                        if (p->pressed) {
                            releaseKey(p->mapped, p->mod);
                            p->pressed = false;
                        }
                    }
                    // TODO: 同時押しされている(本来の)キーをプレス状態にする必要があるか？
                }
                m_henkan_state = false;
            } else {
                m_henkan_state = true;
                m_henkan_only = g_henkan_only_to_henkan;
            }
            return true;
        }
        if (m_henkan_state) {
            m_henkan_only = false;
            for (HenkanKey* p = m_henkan_keys; p->code >= 0; ++p) {
                if (p->code == input->code) {
                    if (input->value == 0) {
                        DP(("HENKAN PRESS OFF:(%d -> %d)\n", p->code, p->mapped));
                        releaseKey(p->mapped, p->mod);
                        p->pressed = false;
                        return true;
                    } else if (input->value > 0) {
                        DP(("HENKAN PRESS ON:(%d -> %d)\n", p->code, p->mapped));
                        pressKey(p->mapped, p->mod);
                        p->pressed = true;
                        return true;
                    }
                }
            }
        }
    }

    if (g_enable_muhenkan_map) {
        if (input->code == KEY_MUHENKAN) {
            if (input->value == 0) {
                if (m_muhenkan_only) {
                    DP(("MUHENKAN TYPE\n"));
                    typeKey(KEY_MUHENKAN, -1);
                } else {
                    DP(("MUHENKAN_TO_LALT RELEASE\n"));
                    releaseKey(KEY_LEFTALT, -1);
                }
                m_muhenkan_state = false;
            } else {
                DP(("MUHENKAN ON\n"));
                m_muhenkan_state = true;
                m_muhenkan_only = true;
            }
            return true;
        }
        if (m_muhenkan_state && m_muhenkan_only) {
            DP(("MUHENKAN_TO_LALT PRESS"));
            m_muhenkan_only = false;
            pressKey(KEY_LEFTALT, -1);
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
