mkdir .\build\windows
set path_include=%JAVA_HOME%\include
set libusb_path=f:\libusb.lib
cl /LD /EHsc /D__WIN__ /I "%path_include%" /I "%path_include%/win32" /I "headers/windows" /I../../com.stericsson.sdk.equipment.io.uart/jni/src/ src/USBDevice.cpp  src/USBHandler.cpp  src/libusb_jni.cpp ../../com.stericsson.sdk.equipment.io.uart/jni/src/Logger.cpp src/util.cpp %libusb_path% /link /DLL /OUT:build/windows/usb_jni.dll /IMPLIB:build/windows/usb_jni.lib
