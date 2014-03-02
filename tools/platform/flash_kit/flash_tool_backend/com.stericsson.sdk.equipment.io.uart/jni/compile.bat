mkdir .\build\windows
set path_include=%JAVA_HOME%\include
cl /LD /EHsc /D__WIN__ /I "%path_include%" /I "%path_include%/win32" /I "headers/windows" src/COMPort.cpp  src/libuart_jni.cpp  src/util.cpp src/Logger.cpp /link /DLL /OUT:build/windows/uart_jni.dll /IMPLIB:build/windows/uart_jni.lib
