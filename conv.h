/*
 * xkeymacs substitute for X11/Linux
 * Copyright 2011 (C) Takeshi Yashiro <yashiromann@nifty.com>
 *
 * conv.h: keyboard input converter (abstract)
 */

#ifndef _DEFINE_H_X11KEYMACS_CONV_
#define _DEFINE_H_X11KEYMACS_CONV_

#include <linux/input.h>
#include <queue>

class Converter
{
public:
    Converter();
    virtual ~Converter();

    bool addInput(struct input_event* input) {
        return handleInput(input);
    }
    bool hasOutput() const {
        return !m_output.empty();
    }
    bool getOutput(struct input_event* output);

protected:
    virtual bool handleInput(struct input_event* input) = 0;
    void addOutput(struct input_event* output);

private:
    std::queue<struct input_event> m_output;
};

#endif /* _DEFINE_H_X11KEYMACS_CONV_ */

