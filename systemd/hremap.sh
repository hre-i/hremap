#!/bin/sh

### Keyboard devices
DEV=
for d in \
    /dev/input/by-id/usb-ErgoDox_EZ_ErgoDox_EZ_0-event-kbd \
    /dev/input/by-id/usb-ErgoDox_EZ_ErgoDox_EZ_0-if03-event-kbd \
    /dev/input/by-id/usb-ErgoDox_EZ_ErgoDox_EZ-event-kbd \
    /dev/input/by-id/usb-PFU_Limited_HHKB_Professional_JP-event-kbd \
    /dev/input/by-id/usb-Topre_Corporation_RealForce_Compact-event-kbd \
    /dev/input/by-path/platform-i8042-serio-0-event-kbd \
    /dev/input/event2
do
    if [ -c "$d" -a -z "$DEV" ]; then
	DEV="$d"
    fi
done

if [ ! -z "$DEV" ]; then
    case x"`pidof x11hremap`" in
        x[0-9]*) sudo killall hremap ;;
        *) ;;
    esac
    while true; do
	/usr/local/bin/hremap --enable-function-map --enable-ctrl-map --jp-to-us --caps-to-ctrl $DEV
	sleep 2
    done
else
    echo "NO $DEV"
    exit 1
fi
