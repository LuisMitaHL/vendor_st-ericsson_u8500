mkdir .\build\windows
set path_include=%JAVA_HOME%\include
cl /LD /EHsc /D__WIN__ /I "%path_include%" /I "%path_include%/win32" /I "headers/windows" /I../../com.stericsson.sdk.equipment.io.uart/jni/src/ src/JNInative.cpp src/util.cpp ../../com.stericsson.sdk.equipment.io.uart/jni/src/Logger.cpp /link /DLL /OUT:build/windows/lc_jni.dll /IMPLIB:build/windows/lc_jni.lib
