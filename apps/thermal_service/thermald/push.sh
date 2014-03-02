#!/bin/sh

adb remount
adb push thermal.conf /system/etc
adb push ../../../../../out/target/product/u8500/symbols/system/bin/thermald /system/bin
