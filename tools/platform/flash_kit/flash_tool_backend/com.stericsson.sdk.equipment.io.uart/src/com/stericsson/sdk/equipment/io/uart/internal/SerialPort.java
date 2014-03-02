package com.stericsson.sdk.equipment.io.uart.internal;

import static com.stericsson.sdk.common.configuration.ConfigurationOption.MONITOR;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_ALIAS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_BAUDRATE;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_DATABITS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_ENABLED;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_PARITY;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_STOPBIT;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_TRIGGER;
import static com.stericsson.sdk.equipment.Activator.getServiceObject;
import static com.stericsson.sdk.equipment.Activator.getServiceObjects;

import java.util.ArrayList;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.List;

import org.apache.log4j.Logger;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.equipment.io.port.IOSimulator;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortProvider;
import com.stericsson.sdk.equipment.io.port.IPortProviderTester;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.uart.Activator;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.BAUD_RATE;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.DATA_BITS;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.PARITY;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.STOP_BITS;

/**
 * Serial port (uart) implementation.
 * 
 * This class wraps a physical serial port (and thus this class is partially native).
 * 
 * Most of the methods in this class must be used through proxy class (SerialPortProxy). Only one
 * proxy instance is allowed to call methods in this class (proxy owns an object of this class). If
 * other proxy (not owner) calls a method, an exception is thrown. New owner (proxy) of this class
 * is created when port is triggered (trigger method).
 * 
 * @author rohacmic
 * 
 */
public final class SerialPort implements IPortProvider, IPortProviderTester, IConfigurationServiceListener {

    private static final Logger log = Logger.getLogger(SerialPort.class);

    private final String identifier;

    private SerialPortConfiguration configuration;

    ServiceRegistration registeredService;

    private IOSimulator ioSimulator;

    private boolean opened;

    private boolean acceptEquipments;

    private SerialPortConfiguration changedConfiguration = null;

    /**
     * Only owner is allowed to perform operations on this port.
     */
    private SerialPortProxy owner = null;

    // Will be instantiated from JNI layer
    private SerialPort(String pIdentifier) {
        identifier = pIdentifier;
        if (EnvironmentProperties.isRunningInTestMode()) {
            SerialPortHandler.jniWarn("UART device created in testing mode..");
        }
    }

    /**
     * {@inheritDoc}
     */
    public String getIdentifier() {
        return identifier;
    }

    /**
     * @return Configuration for this port.
     */
    public SerialPortConfiguration getConfiguration() {
        return configuration;
    }

    /**
     * Will open port for I/O operation.
     * 
     * @param caller
     *            proxy calling this method
     * @throws UARTException
     *             When I/O error occurs.
     */
    public void open(SerialPortProxy caller) throws UARTException {
        if (caller != owner) {
            throw new UARTException("Port " + identifier + " is already in use (was captured by another task)");
        }
        if (!EnvironmentProperties.isRunningInTestMode()) {
            openPort();
        } else {
            SerialPortHandler.jniDebug("Opening port in testing mode..");
            opened = true;
        }
        if (changedConfiguration != null && !changedConfiguration.equals(configuration)) {
            set(caller, changedConfiguration);
        }
    }

    /**
     * Will configure this port with given configuration.
     * 
     * @param caller
     *            proxy calling this method
     * @param config
     *            Configuration which should be applied to this port.
     * @throws UARTException
     *             When setting configuration fails.
     */
    void set(SerialPortProxy caller, SerialPortConfiguration config) throws UARTException {
        if (caller != owner) {
            throw new UARTException("Port " + identifier + " is already in use (was captured by another task)");
        }
        if (!EnvironmentProperties.isRunningInTestMode()) {
            log.debug("Calling native uart set method with speed " + config.baudRate);
            setPort(config);
        } else {
            SerialPortHandler.jniInfo("UART configuration set in testing mode..");
        }
        this.configuration = config;
    }

    /**
     * Closes serial port.
     * 
     * @param caller
     *            proxy calling this method
     * @throws UARTException
     *             When I/O error occurs.
     */
    public void close(SerialPortProxy caller) throws UARTException {
        if (caller != owner) {
            throw new UARTException("Port " + identifier + " is already in use (was captured by another task)");
        }
        if (!EnvironmentProperties.isRunningInTestMode()) {
            closePort();
        } else {
            opened = false;
        }
    }

    /**
     * Checks if port is opened.
     * 
     * @param caller
     *            proxy calling this method
     * 
     * @return True if port is open.
     */
    public boolean isOpen(SerialPortProxy caller) {
        if (caller != owner) {
            // even if port is opened, it is not opened for the caller
            return false;
        }
        if (!EnvironmentProperties.isRunningInTestMode()) {
            return portOpened();
        } else {
            return opened;
        }
    }

    /**
     * Will try to read 'length' number of bytes to given buffer from given offset.
     * 
     * @param caller
     *            proxy calling this method
     * @param buf
     *            Buffer to fill with read data.
     * @param offset
     *            Offset into buffer from which data should be stored.
     * @param length
     *            Number of bytes to be read.
     * @return Number of bytes actually read.
     * @throws UARTException
     *             When I/O error occurs.
     * @throws PortException
     *             TBD
     */
    public int read(SerialPortProxy caller, byte[] buf, int offset, int length) throws UARTException, PortException {
        if (caller != owner) {
            throw new UARTException("Port " + identifier + " is already in use (was captured by another task)");
        }
        if (!isOpen(caller)) {
            SerialPortHandler.jniError("Port has to be opened before first IO operation!");
            throw new PortException("Port is not opened!");
        } else {
            if (EnvironmentProperties.isRunningInTestMode()) {
                return ioSimulator.read(buf, offset, length);
            } else {
                return portRead(buf, offset, length);
            }
        }
    }

    /**
     * Will try to write 'length' number of bytes from offset in given buffer.
     * 
     * @param caller
     *            proxy calling this method
     * @param buf
     *            Buffer with data to write.
     * @param offset
     *            Offset into given buffer from which data will written.
     * @param length
     *            Number of bytes to be written.
     * @return Number of bytes actually written.
     * @throws UARTException
     *             When I/O error occurs.
     * @throws PortException
     *             TBD
     */
    public int write(SerialPortProxy caller, byte[] buf, int offset, int length) throws UARTException, PortException {
        if (caller != owner) {
            throw new UARTException("Port " + identifier + " is already in use (was captured by another task)");
        }
        if (!isOpen(caller)) {
            SerialPortHandler.jniError("Port has to be opened before first IO operation!");
            throw new PortException("Port is not opened!");
        } else {
            if (EnvironmentProperties.isRunningInTestMode()) {
                return ioSimulator.write(buf, offset, length);
            } else {
                return portWrite(buf, offset, length);
            }
        }
    }

    private native void openPort() throws UARTException;

    private native void setPort(SerialPortConfiguration config) throws UARTException;

    private native void closePort();

    private native boolean portOpened();

    private native int portRead(byte[] buf, int offset, int length) throws UARTException;

    private native int portWrite(byte[] buf, int offset, int length) throws UARTException;

    /**
     * Flushes port. On Windows this method hangs till all the transfers are complete. On Linux this
     * method flushes pending transfers (drops UART buffer) and returns immediately.
     * 
     * @return 0 on success
     * @throws UARTException
     */
    private native int portFlush() throws UARTException;

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return getIdentifier();
    }

    /**
     * {@inheritDoc}
     */
    public boolean isTriggered() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public void release() {
        // Nothing TO DO here..
        return;
    }

    /**
     * {@inheritDoc}
     */
    public void trigger() throws PortException {
        try {
            IPortReceiver portReciever = null;
            if (!EnvironmentProperties.isRunningInTestMode()) {
                portReciever = getServiceObject(IPortReceiver.class);
            } else {
                List<IPortReceiver> recievers =
                    getServiceObjects(IPortReceiver.class, "(port.reciever.name="
                        + IPortProviderTester.UART_PORT_RECIEVER_NAME + ")");
                if ((recievers != null) && (recievers.size() > 0)) {
                    portReciever = recievers.get(0);
                }
            }

            if (portReciever != null) {
                owner = new SerialPortProxy(this);
                if (isOpen(owner)) {
                    if (0 != portFlush()) {
                        throw new PortException("Flushing failed");
                    }

                    log.debug("Flushing port " + identifier + " successfull");
                }
                new PortDeliver(portReciever, owner).deliverAsync();
            } else {
                log.error("No port reciever found for delivery of '" + getIdentifier() + "' port!");
            }
        } catch (Exception e) {
            log.error("Port triggering failed!", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {

        if (newRecord.getName().equalsIgnoreCase(PORT_UART)) {
            if (newRecord.isValueNameSet(PORT_UART_ALIAS)
                && (newRecord.getValue(PORT_UART_ALIAS).equalsIgnoreCase(getIdentifier()) || newRecord.getValue(
                    PORT_UART_ALIAS).equalsIgnoreCase(ConfigurationOption.AUTO_UART_PORT_ID))) {
                try {
                    configurePort(newRecord);
                } catch (UARTException e) {
                    log.error("Setting port configuration failed for port '" + getIdentifier() + "'!", e);
                } catch (PortException e) {
                    log.error("Triggering port '" + getIdentifier() + "' failed!", e);
                }
            }
        } else if (newRecord.getName().equalsIgnoreCase(MONITOR)) {
            if (newRecord.isValueNameSet(MONITOR_ACCEPT_EQUIPMENTS)) {
                this.acceptEquipments = Boolean.parseBoolean(newRecord.getValue(MONITOR_ACCEPT_EQUIPMENTS));
            } else {
                this.acceptEquipments = false;
            }
        }
    }

    /**
     * This method takes an record and do one of following, set new port configuration, trigger port
     * or do nothing in case of bad configuration. After setting valid UART configuration
     * IPortProvider is always registered. In case of invalid configuration record or invalid UART
     * configuration IPortProvider is always unregistered. Port is triggered only in case that
     * acceptEquipment is allowed, valid configuration has been already set and given record force
     * it.
     * 
     * @param record
     *            Configuration record.
     * @throws UARTException
     *             When setting port configuration failed.
     * @throws PortException
     *             When port is triggered and port delivery failed.
     */
    private void configurePort(IConfigurationRecord record) throws UARTException, PortException {
        if (record.isValueNameSet(PORT_UART_ENABLED) && !"true".equalsIgnoreCase(record.getValue(PORT_UART_ENABLED))) {
            return;
        }

        BAUD_RATE br = BAUD_RATE.getByString(record.getValue(PORT_UART_BAUDRATE));
        DATA_BITS db = DATA_BITS.getByString(record.getValue(PORT_UART_DATABITS));
        PARITY par = PARITY.getByString(record.getValue(PORT_UART_PARITY));
        STOP_BITS sbits = STOP_BITS.getByString(record.getValue(PORT_UART_STOPBIT));

        if (checkConfiguration(br, db, par, sbits)) {
            changedConfiguration = new SerialPortConfiguration(br, db, par, sbits);
            if (registeredService == null) {
                Dictionary<String, String> settings = new Hashtable<String, String>();
                settings.put(IPortProvider.PORT_PROVIDER_NAME, getIdentifier());
                registeredService =
                    Activator.getBundleContext().registerService(IPortProvider.class.getName(), this, settings);
            }
        } else if (record.isValueNameSet(PORT_UART_TRIGGER)) {
            if (acceptEquipments && Boolean.parseBoolean(record.getValue(PORT_UART_TRIGGER))
                && (registeredService != null)) {
                trigger();
            }
        } else {
            log.error("Bad configuration record for port '" + getIdentifier() + "'. Record: " + record.toString());
            if (registeredService != null) {
                registeredService.unregister();
                registeredService = null;
            }
        }
    }

    private boolean checkConfiguration(BAUD_RATE br, DATA_BITS db, PARITY par, STOP_BITS sb) {
        return ((br != null) && (db != null) && (par != null) && (sb != null));
    }

    /**
     * {@inheritDoc}
     */
    public void simulateUSBDevice(int pVid, int pPid, byte[][] pReadData, byte[][] pWriteData) {
        throw new UnsupportedOperationException();
    }

    /**
     * {@inheritDoc}
     */
    public void simulateUARTDevice(byte[][] pReadData, byte[][] pWriteData) {
        SerialPortHandler.jniInfo("UART communication testing mode..");
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
     * @return Dummy port for testing purpose.
     */
    public static SerialPort getDummySerialPort() {
        return new SerialPort(DUMMY_SERIAL_PORT);
    }
}

/**
 * Class to deliver port asynchronously.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
class PortDeliver implements Runnable {

    private IPort owner;

    private IPortReceiver portReciever;

    public PortDeliver(IPortReceiver pPortReciver, IPort pOwner) {
        owner = pOwner;
        portReciever = pPortReciver;
    }

    public void deliverAsync() {
        new Thread(this).start();
    }

    /**
     * {@inheritDoc}
     */
    public void run() {
        try {
            portReciever.deliverPort(owner, PortDeliveryState.SERVICE);
        } catch (PortException e) {
            e.printStackTrace();
        }
    }

}
