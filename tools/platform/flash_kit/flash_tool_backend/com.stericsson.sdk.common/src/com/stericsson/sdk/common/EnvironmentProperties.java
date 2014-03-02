package com.stericsson.sdk.common;

import java.util.Locale;

/**
 * @author xmicroh
 * 
 *         Utility class for fetching basic environment properties.
 */
public final class EnvironmentProperties {

    private static final String BACKEND_MODE = "backend.mode";

    private static final Object BACKEND_MODE_TEST = "test";

    private EnvironmentProperties() {
    }

    /**
     * Checks if program is being run on a 64-bit machine.
     * 
     * @return True when JVM is running in 64-bit mode, false when it 32-bit mode or detection
     *         failed.
     */
    public static boolean is64BitVM() {
        String bits = System.getProperty("sun.arch.data.model", "?");
        if (bits.equals("64")) {
            return true;
        }
        if (bits.equals("?")) {
            // probably sun.arch.data.model isn't available
            // maybe not a Sun JVM?
            // try with the vm.name property
            return System.getProperty("java.vm.name").toLowerCase(Locale.getDefault()).indexOf("64") >= 0;
        }
        // probably 32bit
        return false;
    }

    /**
     * Checks if program is being run on a Windows system.
     * 
     * @return True when JVM is running on Windows OS, false otherwise.
     */
    public static boolean isWindows() {

        String os = System.getProperty("os.name").toLowerCase(Locale.getDefault());
        // windows
        return (os.indexOf("win") >= 0);
    }

    /**
     * Checks if program is being run on a Mac system.
     * 
     * @return True when JVM is running on Mac OS, false otherwise.
     */
    public static boolean isMac() {

        String os = System.getProperty("os.name").toLowerCase(Locale.getDefault());
        // Mac
        return (os.indexOf("mac") >= 0);
    }

    /**
     * Checks if program is being run on a Unix system.
     * 
     * @return True when JVM is running on Unix based OS, false otherwise.
     */
    public static boolean isUnix() {

        String os = System.getProperty("os.name").toLowerCase(Locale.getDefault());
        // linux or unix
        return (os.indexOf("nix") >= 0 || os.indexOf("nux") >= 0);
    }

    /**
     * Checks if program is running in test mode. Program is running in test mode if "backend.mode"
     * system property is defined and equals "test". If program is running in test mode
     * LoaderCommunicationMock is used instead of regular LCD. Also fake devices get started (except
     * for DummyDevice, this is a separate osgi service).
     * 
     * @return true if backend is running in test mode
     */
    public static boolean isRunningInTestMode() {
        return (System.getProperty(BACKEND_MODE) != null && System.getProperty(BACKEND_MODE).equals(BACKEND_MODE_TEST));
    }

    /**
     * This method return a string contains OS dynamic library extension
     * 
     * @return string contains dynamic library extension
     */
    public static String getLibraryExtension() {
        if (isMac()) {
            return ".jnilib";
        } else if (isWindows()) {
            return ".dll";
        } else if (isUnix()) {
            return ".so";
        } else {
            return "";
        }
    }
}
