#!/bin/sh

adb wait-for-device
adb shell am instrument -w com.stericsson.thermal.test/android.test.InstrumentationTestRunner
