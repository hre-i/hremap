#!/bin/sh

### Keyboard devices
DEV=
for d in \
    /dev/input/event3
do
    if [ -c "$d" -a -z "$DEV" ]; then
	DEV="$d"
    fi
done

opt="--ctrl-map --f11-to-henkan --f12-to-katakana --katakana-map --henkan-map"

if [ ! -z "$DEV" ]; then
    case x"`pidof x11hremap`" in
        x[0-9]*) sudo killall hremap ;;
        *) ;;
    esac
    while true; do
	/usr/local/bin/hremap ${opt} $DEV
	sleep 2
    done
else
    echo "NO $DEV"
    exit 1
fi
