package com.stericsson.sdk.equipment.io.uart.internal;

import static com.stericsson.sdk.common.EnvironmentProperties.getLibraryExtension;
import static com.stericsson.sdk.common.EnvironmentProperties.is64BitVM;
import static com.stericsson.sdk.common.EnvironmentProperties.isMac;
import static com.stericsson.sdk.common.EnvironmentProperties.isUnix;
import static com.stericsson.sdk.common.EnvironmentProperties.isWindows;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.common.configuration.system.SystemProperties;

/**
 * @author rohacmic
 * 
 */
public final class SerialPortHandler {

    private static final Logger log = Logger.getLogger(SerialPortHandler.class);

    private static final SerialPortHandler INSTANCE = new SerialPortHandler();

    private static final String LIB_NAME = "uart_jni";

    private static final String LIB_NAME_X64 = "uart_jni_x64";

    private static final int JNI_LOG_LEVEL = 0;

    private static final String JNI_LOG_PREFIX = "JNI: ";

    private List<SerialPort> serialPorts = null;

    /**
     * @return Instance of this singleton.
     */
    public static SerialPortHandler getInstance() {
        return INSTANCE;
    }

    private SerialPortHandler() {
        try {
            if (!EnvironmentProperties.isRunningInTestMode()) {
                String libNativeName = getNativeLibName();
                System.loadLibrary(libNativeName);
                log.debug("Serial library Loaded: " + libNativeName + getLibraryExtension());

                initJNILayer(getLogLevel());
            }
        } catch (Throwable t) {
            log.error("JNI UART layer initialization failed!", t);
        }

        serialPorts = new ArrayList<SerialPort>();
        if (!EnvironmentProperties.isRunningInTestMode()) {
            Collections.addAll(serialPorts, getAvailablePorts());
        } else {
            serialPorts.add(SerialPort.getDummySerialPort());
        }
    }

    private static String getNativeLibName() throws Exception {
        if (isUnix() || isWindows() || isMac()) {
            if (is64BitVM()) {
                return LIB_NAME_X64;
            } else {
                return LIB_NAME;
            }
        } else {
            throw new Exception("Could not detect system type (system must be Unix, Windows or Mac OS)");
        }
    }

    /**
     * Method return value related to system setting of jni log.
     * 
     * @return Level of jni log.
     */
    private static int getLogLevel() {
        String level = SystemProperties.getProperty(SystemProperties.JNI_LOG_LEVEL);
        int result = 0;
        if (level != null) {
            result = Integer.parseInt(level);
            if (result < 0 || result > 4) {
                log.warn("Jni log level <" + result + "> is out of range!");
                return JNI_LOG_LEVEL;
            } else {
                return result;
            }
        } else {
            return JNI_LOG_LEVEL;
        }
    }

    /**
     * @return List of available ports.
     */
    public List<SerialPort> getSerialPorts() {
        return serialPorts;
    }

    static void jniError(String msg) {
        log.error(JNI_LOG_PREFIX + msg);
    }

    static void jniWarn(String msg) {
        log.warn(JNI_LOG_PREFIX + msg);
    }

    static void jniInfo(String msg) {
        log.info(JNI_LOG_PREFIX + msg);
    }

    static void jniDebug(String msg) {
        log.debug(JNI_LOG_PREFIX + msg);
    }

    private native void initJNILayer(int jniLogLevel) throws UARTException;

    private native SerialPort[] getAvailablePorts();

}
