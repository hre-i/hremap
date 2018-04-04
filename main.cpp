/*
 * hremap -- key remapper for Linux
 * (C) IMAI Hiroei
 *
 * xkeymacs substitute for X11/Linux
 * Copyright 2011 (C) Takeshi Yashiro <yashiromann@nifty.com>
 *
 * main.cpp: main routine
 */

#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <memory>
#include <string>
#include <vector>
#include "keyboard.h"
#include "conv.h"
#include "hremap.h"

static void usage()
{
    fprintf(stderr, "Usage: hremap <options> /dev/input/event<n>\n");
    fprintf(stderr, "  -C|--caps-to-ctrl: convert caps to left ctrl\n");
    fprintf(stderr, "  -c|--enable-ctrl-map: enable ctrl+{h,m} mapping\n");
    fprintf(stderr, "  -f|--enable-function-map: enable mappings with function keys\n");
    fprintf(stderr, "  -n|--no-grab: do not grab device input (for debug)\n");
    fprintf(stderr, "  -d|--debug: enable debug mode\n");
}

bool g_debug = false;
bool g_enable_ctrl_map = false;
bool g_enable_function_map = false;
bool g_caps_to_ctrl = false;
bool g_jp_to_us = false;

static void sighandle(int signal)
{
    g_debug = !g_debug;
}

int main(int argc, char* argv[])
{
    char* fileName = NULL;
    bool noGrabMode = false;

    g_debug = false;

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-C") == 0
                || strcmp(argv[i], "--caps-to-ctrl") == 0) {
                g_caps_to_ctrl = true;
            }
            else if (strcmp(argv[i], "-j") == 0
                || strcmp(argv[i], "--jp-to-us") == 0) {
                g_jp_to_us = true;
            }
            else if (strcmp(argv[i], "-c") == 0
                || strcmp(argv[i], "--enable-ctrl-map") == 0) {
                g_enable_ctrl_map = true;
            }
            else if (strcmp(argv[i], "-f") == 0
                || strcmp(argv[i], "--enable-function-map") == 0) {
                g_enable_function_map = true;
            }
            else if (strcmp(argv[i], "-n") == 0
                || strcmp(argv[i], "--no-grab") == 0) {
                noGrabMode = true;
            }
            else if (strcmp(argv[i], "-d") == 0
                || strcmp(argv[i], "--debug") == 0) {
                g_debug = true;
            }
            else {
                fprintf(stderr, "ERROR: bad option `%s'\n", argv[i]);
                usage();
                return 1;
            }
        }
        else {
            if (fileName != NULL) {
                fprintf(stderr, "ERROR: multiple targets given\n");
                usage();
                return 1;
            }
            fileName = argv[i];
        }
    }
    if (fileName == NULL) {
        fprintf(stderr, "ERROR: device file not specified\n");
        usage();
        return 1;
    }
    // if (!WindowSystem::getInstance()->isValid()) {
    //     fprintf(stderr, "ERROR: cannot open display\n");
    //     return 1;
    // }

    /* We don't want keyboard echos */
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) >= 0) {
        fprintf(stderr, "Disable keyboard echos\n");
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSADRAIN, &term);
    }

    /* Initialize keyboard device */
    std::unique_ptr<KeyboardDevice>
        device(new KeyboardDevice(fileName, !noGrabMode));
    if (!device->isKeyDevValid()) {
        fprintf(stderr, "ERROR: cannot open device `%s\'\n", fileName);
        return 1;
    }
    if (!device->isUserDevValid()) {
        fprintf(stderr, "ERROR: device open device `%s\'\n", USERDEV_FILENAME);
        return 1;
    }

    /* Initialize converter */
    std::unique_ptr<Converter> conv(new HreMapConverter());

    struct sigaction sa;
    sa.sa_handler = &sighandle; // Setup the sighub handler
    sa.sa_flags = SA_RESTART; // Restart the system call, if at all possible
    sigfillset(&sa.sa_mask);  // Block every signal during the handler
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Signal hander");
    }

    /* Get event, convert, and put the generated events */
    bool prev_syn = false;
    for (;;) {
        struct input_event key;
        device->getKey(&key);
        conv->addInput(&key);
        while (conv->hasOutput()) {
            conv->getOutput(&key);
            if (key.type == EV_SYN && prev_syn) {
                ; // skip dup syn
            } else {
                device->putKey(&key);
                prev_syn = (key.type == EV_SYN);
            }
        }
    }

    return 0;
}
