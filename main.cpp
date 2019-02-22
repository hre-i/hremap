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
    fprintf(stderr, "  --ctrl-map: enable ctrl+{h,m} mapping\n");
    fprintf(stderr, "  --henkan-map: enable mappings with henkan key\n");
    fprintf(stderr, "  --henkan-only-to-henkan: map henkan-only to henkan key\n");
    fprintf(stderr, "  --muhenkan-map: enable mappings with muhenkan key\n");
    fprintf(stderr, "  --jp-to-us: enable mappings for jp keyboard\n");
    fprintf(stderr, "  --hhk-jp-to-us: enable mappings for hhk-jp keyboard\n");
    fprintf(stderr, "  --lalt-to-esc: enable mapping left-alt to esc\n");
    fprintf(stderr, "  --ralt-to-kana: enable mapping left-alt to esc\n");
    fprintf(stderr, "  --no-grab: do not grab device input (for debug)\n");
    fprintf(stderr, "  --debug: enable debug mode\n");
}

bool g_debug = false;
bool g_enable_ctrl_map = false;
bool g_enable_henkan_map = false;
bool g_enable_muhenkan_map = false;
bool g_enable_katakana_map = false;
bool g_lalt_to_esc = false;
bool g_jp_to_us = false;
bool g_hhk_jp_to_us = false;
bool g_muhenkan_to_kana = false;
bool g_muhenkan_to_esc = false;
bool g_henkan_only_to_henkan = false;
bool g_ralt_to_kana = false;

#define DP(x) if (g_debug) printf x

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
            if (strcmp(argv[i], "-h") == 0
                || strcmp(argv[i], "--hhk-jp-to-us") == 0) {
                g_hhk_jp_to_us = true;
            }
            else if (strcmp(argv[i], "-j") == 0
                || strcmp(argv[i], "--jp-to-us") == 0) {
                g_jp_to_us = true;
            }
            else if (strcmp(argv[i], "--ctrl-map") == 0) {
                g_enable_ctrl_map = true;
            }
            else if (strcmp(argv[i], "--henkan-map") == 0) {
                g_enable_henkan_map = true;
            }
            else if (strcmp(argv[i], "--henkan-only-to-henkan") == 0) {
                g_henkan_only_to_henkan = true;
            }
            else if (strcmp(argv[i], "--muhenkan-map") == 0) {
                g_enable_muhenkan_map = true;
            }
            else if (strcmp(argv[i], "--katakana-map") == 0) {
                g_enable_katakana_map = true;
            }
            else if (strcmp(argv[i], "--lalt-to-esc") == 0) {
                g_lalt_to_esc = true;
            }
            else if (strcmp(argv[i], "--muhenkan-to-kana") == 0) {
                g_muhenkan_to_kana = true;
            }
            else if (strcmp(argv[i], "--muhenkan-to-esc") == 0) {
                g_muhenkan_to_esc = true;
            }
            else if (strcmp(argv[i], "--ralt-to-kana") == 0) {
                g_ralt_to_kana = true;
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
            } else if (key.type == LOCAL_EV_SLEEP) {
                DP(("SLEEP\n"));
                usleep(100000);
            } else {
                device->putKey(&key);
                prev_syn = (key.type == EV_SYN);
            }
        }
    }

    return 0;
}
