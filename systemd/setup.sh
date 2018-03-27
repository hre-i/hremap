#!/bin/sh

cp hremap.sh /usr/local/etc/hremap.sh
chmod 755 /usr/local/etc/hremap.sh

[ -d /usr/lib/systemd/system ] || mkdir /usr/lib/systemd/system
cp hremap.service /usr/lib/systemd/system/hremap.service

systemctl enable hremap
systemctl start hremap
