#!/bin/sh
ifconfig eth0 down
echo "Enabling interface eth0" >> /boot/System/log/$BOOTTIME-kernel
dhcpc -d eth0 < /dev/null >> /boot/System/log/$BOOTTIME-dhcpc 2>&1 &
