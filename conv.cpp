/*
 * xkeymacs substitute for X11/Linux
 * Copyright 2011 (C) Takeshi Yashiro <yashiromann@nifty.com>
 *
 * conv.cpp: keyboard input converter (abstract)
 */

#include <unistd.h>
#include "conv.h"

Converter::Converter()
{
}

Converter::~Converter()
{
}

bool Converter::getOutput(struct input_event* output)
{
    if (m_output.empty())
        return false;
    *output = m_output.front();
    m_output.pop();
    return true;
}

void Converter::addOutput(struct input_event* output, bool setTime)
{
    if (setTime) {
        struct input_event ev = *output;
        gettimeofday(&(ev.time), NULL);
        m_output.push(ev);
    }
    else {
        m_output.push(*output);
    }
}
