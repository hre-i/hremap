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

opt=--enable-ctrl-map
case x"$DEV" in
*ErgoDox*)	
    opt=--enabel-ctrl-map
    ;;
*HHKB*)
    opt="--enable-muhenkan-map --hhk-jp-to-us"
    ;;
*RealForce*)
    opt="--enable-muhenkan-map --enable-function-map --jp-to-us --caps-to-ctrl"
    ;;
esac

if [ ! -z "$DEV" ]; then
    case x"`pidof x11hremap`" in
        x[0-9]*) sudo killall hremap ;;
        *) ;;
    esac
    while true; do
	# /usr/local/bin/hremap --enable-function-map --enable-ctrl-map --hhk-jp-to-us $DEV
	# /usr/local/bin/hremap --enable-function-map --enable-ctrl-map --jp-to-us $DEV
	/usr/local/bin/hremap ${opt} $DEV
	sleep 2
    done
else
    echo "NO $DEV"
    exit 1
fi
