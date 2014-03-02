package com.stericsson.sdk.equipment.io.usb.internal;

import java.util.ArrayList;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.equipment.io.port.IOSimulator;
import com.stericsson.sdk.equipment.io.port.NativeException;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author emicroh
 * 
 */
final class USBNativeDevice {

    static final Logger log = Logger.getLogger(USBNativeDevice.class);

    private static DeviceDescriptor testingDescriptor;
    static {
        testingDescriptor = new DeviceDescriptor();
        testingDescriptor.busDirName = "bus-0";
        testingDescriptor.deviceFileName = "libusb0-dev0";
        testingDescriptor.devNum = 0;

        testingDescriptor.usbDescriptor = new USBDescriptor();
        testingDescriptor.usbDescriptor.manufacturerDesc = "Mobile Equipment Manufacturer";
        testingDescriptor.usbDescriptor.productDesc = "Testing Device";
        testingDescriptor.usbDescriptor.serialNumberDesc = null;
        testingDescriptor.usbDescriptor.idVendor = 0x0;
        testingDescriptor.usbDescriptor.idProduct = 0x0;
        testingDescriptor.usbDescriptor.bLength = 0x12;
        testingDescriptor.usbDescriptor.bDescriptorType = 0x1;
        testingDescriptor.usbDescriptor.bcdUSB = 0x200;
        testingDescriptor.usbDescriptor.bDeviceClass = 0xFF;
        testingDescriptor.usbDescriptor.bDeviceSubClass = 0x0;
        testingDescriptor.usbDescriptor.bDeviceProtocol = 0xFF;
        testingDescriptor.usbDescriptor.bMaxPacketSize0 = 0x40;
        testingDescriptor.usbDescriptor.bcdDevice = 0x100;
        testingDescriptor.usbDescriptor.bNumConfigurations = 0;

        testingDescriptor.configDescriptors = new USBConfigDescriptor[] {
            new USBConfigDescriptor()};
        testingDescriptor.configDescriptors[0].bLength = 0x9;
        testingDescriptor.configDescriptors[0].bDescriptorType = 0x2;
        testingDescriptor.configDescriptors[0].wTotalLength = 32;
        testingDescriptor.configDescriptors[0].bNumInterfaces = 0x1;
        testingDescriptor.configDescriptors[0].bConfigurationValue = 0x1;
        testingDescriptor.configDescriptors[0].configurationDesc = "C1";
        testingDescriptor.configDescriptors[0].bmAttributes = 0xC0;
        testingDescriptor.configDescriptors[0].maxPower = 0x30;
        testingDescriptor.configDescriptors[0].interfaces = new USBInterfaceDescriptor[] {
            new USBInterfaceDescriptor()};

        testingDescriptor.configDescriptors[0].interfaces[0].bLength = 0x9;
        testingDescriptor.configDescriptors[0].interfaces[0].bDescriptorType = 0x4;
        testingDescriptor.configDescriptors[0].interfaces[0].bInterfaceNumber = 0x0;
        testingDescriptor.configDescriptors[0].interfaces[0].bAlternateSetting = 0x0;
        testingDescriptor.configDescriptors[0].interfaces[0].bNumEndpoints = 0x2;
        testingDescriptor.configDescriptors[0].interfaces[0].bInterfaceClass = 0xFF;
        testingDescriptor.configDescriptors[0].interfaces[0].bInterfaceSubClass = 0x0;
        testingDescriptor.configDescriptors[0].interfaces[0].bInterfaceProtocol = 0xFF;
        testingDescriptor.configDescriptors[0].interfaces[0].interfaceDesc = "I1";

        testingDescriptor.configDescriptors[0].interfaces[0].endpoints = new USBEndpointDescriptor[] {
            new USBEndpointDescriptor(), new USBEndpointDescriptor()};
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[0].bLength = 0x7;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[0].bDescriptorType = 0x5;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[0].bEndpointAddress = 0x84;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[0].bmAttributes = 0x2;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[0].wMaxPacketSize = 0x40;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[0].bInterval = 0x0;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[0].bRefresh = 0x0;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[0].bSynchAddress = 0x0;

        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[1].bLength = 0x7;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[1].bDescriptorType = 0x5;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[1].bEndpointAddress = 0x3;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[1].bmAttributes = 0x2;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[1].wMaxPacketSize = 0x40;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[1].bInterval = 0x0;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[1].bRefresh = 0x0;
        testingDescriptor.configDescriptors[0].interfaces[0].endpoints[1].bSynchAddress = 0x0;
    }

    private static final String IDENTIFIER_PREFIX = "USB";

    public static final int DEFAULT_RX_TIMEOUT = 500;

    public static final int DEFAULT_TX_TIMEOUT = 500;

    // private ReentrantLock readLock;
    //
    // private ReentrantLock writeLock;

    private DeviceDescriptor deviceDescriptor;

    private USBDeviceKey key;

    private int logicalIdentifier;

    private IOSimulator ioSimulator;

    private boolean opened = false;

    public USBNativeDevice(int vid, int pid, byte[][] pReadData, byte[][] pWriteData) {
        this(testingDescriptor);
        testingDescriptor.usbDescriptor.idVendor = vid;
        testingDescriptor.usbDescriptor.idProduct = pid;
        USBNativeHandler.getInstance().jniWarn("USB device created in testing mode..");
        List<byte[]> readData = new ArrayList<byte[]>();
        List<byte[]> writeData = new ArrayList<byte[]>();
        if (pReadData != null) {
            for (byte[] data : pReadData) {
                readData.add(data);
            }
        }
        if (pWriteData != null) {
            for (byte[] data : pWriteData) {
                writeData.add(data);
            }
        }

        ioSimulator = new IOSimulator(readData, writeData);
    }

    /**
     * Instance of this class is initiated from JNI code.
     */
    private USBNativeDevice(DeviceDescriptor devDescriptor) {
        this.deviceDescriptor = devDescriptor;
        key =
            new USBDeviceKey(devDescriptor.devNum, devDescriptor.busDirName, devDescriptor.deviceFileName,
                devDescriptor.usbDescriptor.idVendor, devDescriptor.usbDescriptor.idProduct);
        logicalIdentifier = USBNativeHandler.getInstance().getLogicalIdentifier();
        // readLock = new ReentrantLock();
        // writeLock = new ReentrantLock();
    }

    boolean isOpen() {
        return opened;
    }

    void openWrapper() throws NativeException, PortException {
        if (isOpen()) {
            throw new PortException("Port is already opened!");
        }
        if (!EnvironmentProperties.isRunningInTestMode()) {
            open();
        } else {
            USBNativeHandler.getInstance().jniDebug("Opening port in testing mode..");
            opened = true;
        }
    }

    void closeWrapper() throws NativeException, PortException {
        if (!isOpen()) {
            throw new PortException("Port is closed!");
        }
        if (!EnvironmentProperties.isRunningInTestMode()) {
            close();
        } else {
            USBNativeHandler.getInstance().jniDebug("Closing port in testing mode..");
            USBNativeHandler.getInstance().usbDeregistered(key);
            opened = false;
        }
    }

    synchronized native void open() throws NativeException;

    synchronized native void close() throws NativeException;

    private int readWrapper(byte[] data, int offset, int length, int timeout) throws NativeException,
        IOTimeoutException, PortException {
        if (EnvironmentProperties.isRunningInTestMode()) {
            if (!opened) {
                USBNativeHandler.getInstance().jniError("Port has to be opened before first IO operation!");
                throw new NativeException("Port is not opened!");
            } else {
                return ioSimulator.read(data, offset, length);
            }
        } else {
            return read(data, offset, length, timeout);
        }
    }

    private int writeWrapper(byte[] data, int offset, int length, int timeout) throws NativeException,
        IOTimeoutException, PortException {
        if (EnvironmentProperties.isRunningInTestMode()) {
            if (!opened) {
                USBNativeHandler.getInstance().jniError("Port has to be opened before first IO operation!");
                throw new NativeException("Port is not opened!");
            } else {
                return ioSimulator.write(data, offset, length);
            }
        } else {
            return write(data, offset, length, timeout);
        }
    }

    // read/write to USB device is using independent endpoints that's why is better to synchronized
    // these operations using independent reentrant locks
    private native int read(byte[] data, int offset, int length, int timeout) throws NativeException,
        IOTimeoutException;

    private native int write(byte[] data, int offset, int length, int timeout) throws NativeException,
        IOTimeoutException;

    public USBDeviceKey getKey() {
        return key;
    }

    public int getLogicalIdentifier() {
        return logicalIdentifier;
    }

    public int bulkRead(byte[] data, int offset, int length) throws NativeException, IOTimeoutException, PortException {
        return bulkRead(data, offset, length, DEFAULT_RX_TIMEOUT);
    }

    public int bulkRead(byte[] data, int offset, int length, int timeout) throws NativeException, IOTimeoutException,
        PortException {
        //try {
            //if (!Activator.isRunningOnWindows()) {
            //    USBNativeHandler.getInstance().acquireIOLock();
            //}

            return readWrapper(data, offset, length, timeout);
          //} finally {
            //if (!Activator.isRunningOnWindows()) {
            //    USBNativeHandler.getInstance().releaseIOLock();
            //}
        //}
    }

    public int bulkWrite(byte[] data, int offset, int length) throws NativeException, IOTimeoutException, PortException {
        return bulkWrite(data, offset, length, DEFAULT_TX_TIMEOUT);
    }

    public int bulkWrite(byte[] data, int offset, int length, int timeout) throws NativeException, IOTimeoutException,
        PortException {
        //try {
            //if (!Activator.isRunningOnWindows()) {
            //    USBNativeHandler.getInstance().acquireIOLock();
            //}
            // writeLock.lock();
            int written = 0; // writeWrapper(data, offset, length, timeout);
            // for security reason it's good stop sending data after some time, here when attempts >
            // length
            int attempts = 0;
            while ((written < length) && (attempts <= 10)) {
                attempts++;
                int result = 0;

                result = writeWrapper(data, offset + written, length - written, timeout);

                if (result > 0) {
                    written += result;
                }
            }
            return written;
        //} finally {
            //if (!Activator.isRunningOnWindows()) {
            //    USBNativeHandler.getInstance().releaseIOLock();
            //}
        //}
    }

    public String getIdentifier() {
        return IDENTIFIER_PREFIX + logicalIdentifier;
    }

    public String getDeviceInfo() {
        return deviceDescriptor.getPortInfo();
    }

    private static String getAllignedStringValue(String name, Object o, int level) {
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < level; i++) {
            buf.append("\t");
        }

        for (int i = 0; i < 20 - name.length(); i++) {
            buf.append(" ");
        }

        buf.append(name + ": ");
        if (o instanceof Integer) {
            buf.append(o.toString() + "(" + HexUtilities.toHexString(((Integer) o).intValue()) + ")");
        } else {
            buf.append(o.toString());
        }
        buf.append("\n");
        return buf.toString();
    }

    private static class DeviceDescriptor {
        int devNum;

        String busDirName;

        String deviceFileName;

        USBDescriptor usbDescriptor;

        USBConfigDescriptor[] configDescriptors;

        @Override
        public String toString() {
            StringBuffer buf = new StringBuffer();
            // buf.append(" USB device: " + getIdentifier() + "\n");
            buf.append("System path: " + busDirName + deviceFileName + "\n");
            buf.append(getAllignedStringValue("*** USB Descriptor", "", 0));
            buf.append(usbDescriptor.toString(1));
            for (int i = 0; i < configDescriptors.length; i++) {
                buf.append(getAllignedStringValue("*** USB Configuration " + i, "", 0));
                buf.append(configDescriptors[i].toString(1));
            }
            return buf.toString();
        }

        public String getPortInfo() {
            return toString();
        }
    }

    /**
     * @author emicroh
     * 
     */
    private static class USBDescriptor {
        int bLength;

        int bDescriptorType;

        int bcdUSB;

        int bDeviceClass;

        int bDeviceSubClass;

        int bDeviceProtocol;

        int bMaxPacketSize0;

        int idVendor;

        int idProduct;

        int bcdDevice;

        String manufacturerDesc;

        String productDesc;

        String serialNumberDesc;

        int bNumConfigurations;

        public String toString(int level) {
            StringBuffer buf = new StringBuffer();
            buf.append(getAllignedStringValue("Manufacturer", manufacturerDesc == null ? "" : manufacturerDesc, level));
            buf.append(getAllignedStringValue("Product description", productDesc == null ? "" : productDesc, level));
            buf.append(getAllignedStringValue("Serial Number description", serialNumberDesc == null ? ""
                : serialNumberDesc, level));
            buf.append(getAllignedStringValue("bLength", bLength, level));
            buf.append(getAllignedStringValue("bDescriptorType", bDescriptorType, level));
            buf.append(getAllignedStringValue("bcdUSB", bcdUSB, level));
            buf.append(getAllignedStringValue("bDeviceClass", bDeviceClass, level));
            buf.append(getAllignedStringValue("bDeviceSubClass", bDeviceSubClass, level));
            buf.append(getAllignedStringValue("bDeviceProtocol", bDeviceProtocol, level));
            buf.append(getAllignedStringValue("bMaxPacketSize0", bMaxPacketSize0, level));
            buf.append(getAllignedStringValue("idVendor", idVendor, level));
            buf.append(getAllignedStringValue("idProduct", idProduct, level));
            buf.append(getAllignedStringValue("bcdDevice", bcdDevice, level));
            buf.append(getAllignedStringValue("bNumConfigurations", bNumConfigurations, level));
            return buf.toString();
        }

    }

    /**
     * @author emicroh
     * 
     */
    private static class USBConfigDescriptor {
        int bLength;

        int bDescriptorType;

        int wTotalLength;

        int bNumInterfaces;

        int bConfigurationValue;

        String configurationDesc;

        int bmAttributes;

        int maxPower;

        USBInterfaceDescriptor[] interfaces;

        public String toString(int level) {
            StringBuffer buf = new StringBuffer();
            buf.append(getAllignedStringValue("bLength", bLength, level));
            buf.append(getAllignedStringValue("bDescriptorType", bDescriptorType, level));
            buf.append(getAllignedStringValue("wTotalLength", wTotalLength, level));
            buf.append(getAllignedStringValue("bNumInterfaces", bNumInterfaces, level));
            buf.append(getAllignedStringValue("bConfigurationValue", bConfigurationValue, level));
            buf.append(getAllignedStringValue("configurationDesc", configurationDesc, level));
            buf.append(getAllignedStringValue("bmAttributes", bmAttributes, level));
            buf.append(getAllignedStringValue("maxPower", maxPower, level));
            for (int i = 0; i < interfaces.length; i++) {
                buf.append(getAllignedStringValue("*** Interface " + i, "", level));
                buf.append(interfaces[i].toString(level + 1));
            }
            return buf.toString();
        }

    }

    /**
     * @author emicroh
     * 
     */
    private static class USBInterfaceDescriptor {
        int bLength;

        int bDescriptorType;

        int bInterfaceNumber;

        int bAlternateSetting;

        int bNumEndpoints;

        int bInterfaceClass;

        int bInterfaceSubClass;

        int bInterfaceProtocol;

        String interfaceDesc;

        USBEndpointDescriptor[] endpoints;

        public String toString(int level) {
            StringBuffer buf = new StringBuffer();
            buf.append(getAllignedStringValue("bLength", bLength, level));
            buf.append(getAllignedStringValue("bDescriptorType", bDescriptorType, level));
            buf.append(getAllignedStringValue("bInterfaceNumber", bInterfaceNumber, level));
            buf.append(getAllignedStringValue("bAlternateSetting", bAlternateSetting, level));
            buf.append(getAllignedStringValue("bNumEndpoints", bNumEndpoints, level));
            buf.append(getAllignedStringValue("bInterfaceClass", bInterfaceClass, level));
            buf.append(getAllignedStringValue("bInterfaceSubClass", bInterfaceSubClass, level));
            buf.append(getAllignedStringValue("bInterfaceProtocol", bInterfaceProtocol, level));
            buf.append(getAllignedStringValue("interfaceDesc", interfaceDesc, level));
            for (int i = 0; i < endpoints.length; i++) {
                buf.append(getAllignedStringValue("*** Endpoint " + i, "", level));
                buf.append(endpoints[i].toString(level + 1));
            }
            return buf.toString();
        }
    }

    /**
     * @author emicroh
     * 
     */
    private static class USBEndpointDescriptor {
        int bLength;

        int bDescriptorType;

        int bEndpointAddress;

        int bmAttributes;

        int wMaxPacketSize;

        int bInterval;

        int bRefresh;

        int bSynchAddress;

        public String toString(int level) {
            StringBuffer buf = new StringBuffer();
            buf.append(getAllignedStringValue("bLength", bLength, level));
            buf.append(getAllignedStringValue("bDescriptorType", bDescriptorType, level));
            buf.append(getAllignedStringValue("bEndpointAddress", bEndpointAddress, level));
            buf.append(getAllignedStringValue("bmAttributes", bmAttributes, level));
            buf.append(getAllignedStringValue("wMaxPacketSize", wMaxPacketSize, level));
            buf.append(getAllignedStringValue("bInterval", bInterval, level));
            buf.append(getAllignedStringValue("bRefresh", bRefresh, level));
            buf.append(getAllignedStringValue("bSynchAddress", bSynchAddress, level));
            return buf.toString();
        }
    }

}
