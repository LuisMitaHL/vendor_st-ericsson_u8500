package com.stericsson.sdk.equipment.io.usb.internal;

import static com.stericsson.sdk.common.EnvironmentProperties.getLibraryExtension;
import static com.stericsson.sdk.common.EnvironmentProperties.is64BitVM;
import static com.stericsson.sdk.common.EnvironmentProperties.isMac;
import static com.stericsson.sdk.common.EnvironmentProperties.isUnix;
import static com.stericsson.sdk.common.EnvironmentProperties.isWindows;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.locks.ReentrantLock;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.common.configuration.system.SystemProperties;
import com.stericsson.sdk.equipment.io.port.IPortProviderTester;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.NativeException;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.usb.Activator;

/**
 * @author emicroh
 * 
 */
public final class USBNativeHandler {

    static final Logger log = Logger.getLogger(USBNativeHandler.class.getSimpleName());

    private static Map<Integer, List<Integer>> acceptedDevices = new HashMap<Integer, List<Integer>>();

    static {
        // emp
        List<Integer> empPids = new LinkedList<Integer>();
        empPids.add(0x100e);
        // ST Microelectronics
        List<Integer> stPids = new LinkedList<Integer>();
        stPids.add(0x8500);
        // ST-Ericsson
        List<Integer> stePids = new LinkedList<Integer>();
        stePids.add(0x8500);
        stePids.add(0x5500);
        stePids.add(0x5008);
        stePids.add(0x7400);
        stePids.add(0x8520);
        stePids.add(0x9540);
        stePids.add(0x0500);

        // Nokia
        List<Integer> nPids = new LinkedList<Integer>();
        nPids.add(0x5008);
        // Nokia 2
        List<Integer> nPids2 = new LinkedList<Integer>();
        nPids2.add(0x28);
        nPids2.add(0x218);

        acceptedDevices.put(0xBDB, empPids); // A2 MEs
        acceptedDevices.put(0x483, stPids); // MB HW
        acceptedDevices.put(0x4CC, stePids); // MB HW V2
        acceptedDevices.put(0x421, nPids); // Nokia HW V2
        acceptedDevices.put(0x106, nPids2); // Nokia2 HW V2

        // acceptedDevices.put(0x451, 0xD009); //U380
    }

    private static final String JNI_LOG_PREFIX = "JNI: ";

    private static final String LIB_NAME = "usb_jni";

    private static final String LIB_NAME_X64 = "usb_jni_x64";

    private static final boolean[] LOGICAL_IDS = new boolean[128];

    /**
     * Logging level for libusb shared library. Meaningful setting is in range <0, 4> while higher
     * number is more descriptive.
     */
    private static final int LIBUSB_LOG_LEVEL = 0;

    /**
     * Logging level for JNI implementation of USB handling. Meaningful setting is in range <0, 3>
     * while higher number is more descriptive.
     */
    private static final int JNI_LOG_LEVEL = 0;

    private static USBNativeHandler instance;

    private static final long POLL_TIMEOUT = 1000;

    private boolean autosense = false;

    private Thread poolingThread;

    private final Map<USBDeviceKey, USBNativeDevice> usbDevices = new HashMap<USBDeviceKey, USBNativeDevice>();

    private final ReentrantLock ioLock = new ReentrantLock(true);

    private USBNativeHandler() throws Exception {
        // don't load usb libs when running tests
        if (!EnvironmentProperties.isRunningInTestMode()) {
            String libNativeName = getNativeLibName();
            System.loadLibrary(libNativeName);
            log.debug("USB library Loaded: " + libNativeName + getLibraryExtension());

            initJNILayer(getUSBLogLevel(), getLogLevel());
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
     * Method return value related to system setting of libusb log.
     * 
     * @return Level of libusb log.
     */
    private static int getUSBLogLevel() {
        String level = SystemProperties.getProperty(SystemProperties.LIBUSB_LOG_LEVEL);
        int result = 0;
        if (level != null) {
            result = Integer.parseInt(level);
            if (result < 0 || result > 4) {
                log.warn("LibUSB log level <" + result + "> is out of range!");
                return LIBUSB_LOG_LEVEL;
            } else {
                return result;
            }
        } else {
            return LIBUSB_LOG_LEVEL;
        }
    }

    /**
     * Will attempt to acquire lock for IO operation and block until it's hold by another thread.
     * See also: {@link ReentrantLock#lock()}
     */
    public void acquireIOLock() {
        ioLock.lock();
    }

    /**
     * Will release lock acquired for IO operation. See also: {@link ReentrantLock#unlock()}
     */
    public void releaseIOLock() {
        ioLock.unlock();
    }

    /**
     * @return See {@link ReentrantLock#isLocked()}
     */
    public boolean isLocked() {
        return ioLock.isLocked();
    }

    /**
     * Enables pooling over usb bus and starts appropriate pooling thread.
     */
    public synchronized void enableAutosense() {
        if (!autosense) {
            autosense = true;
            log.debug("Start pooling USB device bus changes with POLL_TIMEOUT " + POLL_TIMEOUT + " miliseconds");
            poolingThread = new Thread("USB Polling Thread") {
                @Override
                public void run() {
                    boolean aquired = false;
                    while (autosense) {
                        try {
                            if (!Activator.isRunningOnWindows()) {
                                acquireIOLock();
                            }
                            aquired = true;
                            if (!EnvironmentProperties.isRunningInTestMode()) {
                                checkBusses();
                            }
                            if (!Activator.isRunningOnWindows()) {
                                releaseIOLock();
                            }
                            aquired = false;
                            Thread.sleep(POLL_TIMEOUT);

                        } catch (InterruptedException e) {
                            log.warn("'" + Thread.currentThread().getName()
                                + "' has been intereptud by another thread!", e);
                        } finally {
                            if (aquired) {
                                if (!Activator.isRunningOnWindows()) {
                                    releaseIOLock();
                                }
                            }
                        }
                    }
                }
            };
            poolingThread.setPriority(Thread.MIN_PRIORITY);
            poolingThread.start();
        }
    }

    /**
     * Disables pooling over usb bus and stops appropriate pooling thread.
     */
    public synchronized void disableAutosense() {
        if (poolingThread != null) {
            try {
                if (Thread.currentThread() != poolingThread) {
                    autosense = false;
                    poolingThread.join();
                }
            } catch (InterruptedException e) {
                log.warn("Waiting for dying poolingThread was interrupted!", e);
            } finally {
                poolingThread = null;
            }
        }
    }

    /**
     * @return Retrieve the only instance of this singleton class.
     */
    public static USBNativeHandler getInstance() {
        if (instance == null) {
            try {
                instance = new USBNativeHandler();
            } catch (Exception e) {
                log.error(e.getMessage());
            }
        }
        return instance;
    }

    /**
     * Callback method called from native code to let application know about usb device connection.
     * 
     * @param device
     *            Desired USB device to be registred.
     */
    void registerUSB(USBNativeDevice device) {
        // cache connected device
        usbDevices.put(device.getKey(), device);
        // autosense = false;

        try {
            IPortReceiver portReciever = null;
            if (EnvironmentProperties.isRunningInTestMode()) {
                List<IPortReceiver> portRecievers =
                    com.stericsson.sdk.equipment.Activator.getServiceObjects(IPortReceiver.class,
                        "(port.reciever.name=" + IPortProviderTester.USB_PORT_RECIEVER_NAME + ")");
                if ((portRecievers != null) && (portRecievers.size() == 1)) {
                    portReciever = portRecievers.get(0);
                }
            } else {
                portReciever = com.stericsson.sdk.equipment.Activator.getServiceObject(IPortReceiver.class);
            }

            if (portReciever != null) {
                portReciever.deliverPort(new USBPort(device), PortDeliveryState.SERVICE);
            }
        } catch (PortException e) {
            log.error(e);
        } catch (Throwable t) {
            log.error("Port delivery failed!", t);
        }
    }

    synchronized int getLogicalIdentifier() {
        for (int i = 0; i < LOGICAL_IDS.length; i++) {
            if (!LOGICAL_IDS[i]) {
                LOGICAL_IDS[i] = true;
                return i;
            }
        }
        throw new RuntimeException("No available identifier for next usb device!");
    }

    private synchronized void unsetLogicalIdentifier(int id) {
        LOGICAL_IDS[id] = false;
    }

    /**
     * 
     * Method is called from JNI after device removal from internal cache which is caused by closing
     * device either from java or JNI. USBNativeDevice#close shouldn't be called any more!
     * 
     * @param key
     *            Unique usb device key used also internally in JNI for caching connected devices.
     */
    void usbDeregistered(USBDeviceKey key) {
        IPortReceiver portReciever = com.stericsson.sdk.equipment.Activator.getServiceObject(IPortReceiver.class);
        if (portReciever != null) {
            try {
                portReciever.closePort(new USBPort(usbDevices.get(key)));
            } catch (PortException e) {
                log.error(e.getMessage());
            }
        }

        log.debug("Device has been disconnected: " + usbDevices.get(key).getIdentifier());
        unsetLogicalIdentifier(usbDevices.get(key).getLogicalIdentifier());
        usbDevices.remove(key);
    }

    /**
     * Method called from native code to check if device with given vendor and product ID should be
     * accepted for communication.
     * 
     * @param vendorID
     *            USB device vendor ID.
     * @param productID
     *            USB device product ID.
     * @return True if communication with such device is allowed, false otherwise.
     */
    boolean acceptUSBDevice(int vendorID, int productID) {
        boolean result = false;
        log.debug("Request to accept device: VID=0x" + Integer.toHexString(vendorID) + ", PID=0x"
            + Integer.toHexString(productID));
        List<Integer> list = acceptedDevices.get(vendorID);
        result = list != null && list.contains(productID);
        if (!result) {
            log.debug("Request was rejected!");
        }
        return result;
    }

    boolean autosenseEnabled() {
        return autosense;
    }

    void jniError(String msg) {
        log.error(JNI_LOG_PREFIX + msg);
    }

    void jniWarn(String msg) {
        log.warn(JNI_LOG_PREFIX + msg);
    }

    void jniInfo(String msg) {
        log.info(JNI_LOG_PREFIX + msg);
    }

    void jniDebug(String msg) {
        log.debug(JNI_LOG_PREFIX + msg);
    }

    private native void initJNILayer(int libusbLogLevel, int jniLogLevel) throws NativeException;

    private native void checkBusses();
}
