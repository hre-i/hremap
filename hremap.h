/*
 * hremap -- key remapper for Linux
 * (C) IMAI Hiroei
 *
 * Copyright 2017,2018 (C) IMAI Hiroei
 *
 * conv.h: keyboard input converter (abstract)
 */

#ifndef _DEFINE_H_X11HREMAP__
#define _DEFINE_H_X11HREMAP__

#include <vector>
#include <string>
#include <assert.h>
#include <string.h>
#include <linux/input.h>
#include "conv.h"

class HreMapConverter : public Converter
{
public:
    HreMapConverter();
    virtual ~HreMapConverter();

protected:
    virtual bool handleInput(struct input_event* input);

    bool handleKeyInput(struct input_event* input);
    bool handleMetaKeyInput(struct input_event* input, int bit);
    bool handleKeyDefault(struct input_event* input);
    void typeKey(__u16 code, int metaKeys);
    void pressKey(__u16 code, int metaKeys);
    void releaseKey(__u16 code, int metaKeys);
    void setMetaKeys(int, int);

private:

    static const int   m_metaBits[];
    static const __u16 m_metaKeys[];
    static const char* m_metaKeyNames[];

    struct input_event syn;
    void addSyn() { addOutput(&syn); }
    struct input_event sleep;
    void addSleep() { addOutput(&sleep); }

    struct HenkanKey {
        int code;               // 同時に押すコード
        int mapped;             // 変換後コード
        int mod;                // 変換後 Modifier
        bool pressed;           // 押下状態
    };
    HenkanKey* find_henkan_keys(int code);
    HenkanKey* find_muhen_keys(int code);

    int m_metaKeyFlags;
    bool m_henkan_state;
    bool m_henkan_only;
    bool m_muhen_state;
    bool m_muhen_only;
    static HenkanKey m_henkan_keys[];
    static HenkanKey m_muhen_keys[];
};

#define LOCAL_EV_SLEEP  (192)

#endif /* _DEFINE_H_X11HREMAP__ */

