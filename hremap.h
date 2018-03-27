/*
 * hremap -- key remapper for Linux
 * (C) IMAI Hiroei
 *
 * Copyright 2017 (C) IMAI Hiroei
 *
 * conv.h: keyboard input converter (abstract)
 */

#ifndef _DEFINE_H_X11HREMAP__
#define _DEFINE_H_X11HREMAP__

#include <vector>
#include <string>
#include <linux/input.h>
#include <boost/regex.hpp>
#include "conv.h"

class HreMapConverter : public Converter
{
public:
    HreMapConverter(const std::vector<std::string>& excludeList);
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

    void addExcludePattern(const char* pattern);
    bool exclude() const;

private:

    static const int   m_metaBits[];
    static const __u16 m_metaKeys[];
    static const char* m_metaKeyNames[];

    struct HenkanKey {
        int code;               // 同時に押すコード
        int mapped;             // 変換後コード
        bool pressed;           // 押下状態
    };
    HenkanKey* find_henkan_keys(int code);

    int m_metaKeyFlags;
    bool m_henkan_state;
    static HenkanKey m_henkan_keys[];

    bool m_state_pasteNewline;
    typedef std::vector<boost::regex*> ExcludeList;
    ExcludeList m_excludePatterns;
};


#endif /* _DEFINE_H_X11HREMAP__ */

