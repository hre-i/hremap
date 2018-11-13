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

opt=--ctrl-map
case x"$DEV" in
*ErgoDox*)	
    opt="$opt"
    ;;
*HHKB*)
    opt="--henkan-map --muhenkan-map --katakana-map --muhenkan-to-esc --function-map --hhk-jp-to-us $opt"
    ;;
*RealForce*)
    opt="--henkan-map --muhenkan-map --katakana-map --muhenkan-to-esc --function-map --jp-to-us $opt"
    ;;
*platform-i8042*)
    opt="--henkan-map --muhenkan-map --katakana-map --ralt-to-kana --function-map $opt"
    ;;
esac

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
