#/usr/bin/env bash

if [ -z "$OUT" ]; then
    echo "\$OUT is not set. Android build environment is not setup properly!"
    exit -1
fi

if [ ! -e "$OUT/data/app/IsolatedSTErilOemService.apk" ]; then
    echo "Test package is not built. run 'mm'"
    exit -1
fi

if [ ! -e "$OUT/data/app/STErilOemHookTests.apk" ]; then
    echo "Test package is not built. run 'mm'"
    exit -1
fi

# Make sure a device is connected
adb wait-for-device

# Install the test packages and reboot
adb remount
adb sync
echo "Removing STErilOemService before running module tests"
adb shell rm /system/app/STErilOemService.apk #remove real service during module test
sleep 3
echo "Rebooting device"
adb reboot


# Run the module test
adb wait-for-device
echo "Waiting 20 seconds for device to settle"
sleep 20
echo "Running Module tests"
adb shell am instrument -w -e class com.stericsson.ril.oem.service.test.STErilOemServiceModuleTest \
        com.stericsson.ril.oem.service.test.client/android.test.InstrumentationTestRunner


# Uninstall the test service and reboot
sleep 3  #allow the device to settle
adb remount
adb uninstall com.stericsson.ril.oem.service.test
echo "Re-installing STErilOemService"
adb push "$OUT/system/app/STErilOemService.apk" /system/app # re-install the real service
echo "Rebooting device"
adb reboot


# Run functional tests
adb wait-for-device
echo "Waiting 40 seconds for device to settle" # wait longer since the RIL needs to come online.
sleep 40
echo "Running Functional tests"
adb shell am instrument -w -e class com.stericsson.ril.oem.service.test.STErilOemServiceFuncTest \
        com.stericsson.ril.oem.service.test.client/android.test.InstrumentationTestRunner


echo "Finished!"
echo "Hint: You may want to 'rm -f $OUT/data/app/IsolatedSTErilOemService.apk'"
echo "so that it doesn't accidentally get installed with the next 'adb sync'"
