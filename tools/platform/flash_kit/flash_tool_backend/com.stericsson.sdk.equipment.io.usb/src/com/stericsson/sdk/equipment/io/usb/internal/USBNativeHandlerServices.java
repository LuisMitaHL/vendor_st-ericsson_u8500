package com.stericsson.sdk.equipment.io.usb.internal;

import java.util.Hashtable;

import org.apache.log4j.Logger;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.equipment.io.port.IPortProvider;
import com.stericsson.sdk.equipment.io.port.IPortProviderTester;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.usb.Activator;

/**
 * @author emicroh
 * 
 */
public final class USBNativeHandlerServices implements IPortProvider, IPortProviderTester,
    IConfigurationServiceListener {

    static final Logger log = Logger.getLogger(USBNativeHandlerServices.class);

    private static final String PORT_PROVIDER_IDENTIFIER = "USB";

    private USBNativeHandler nativeHandler;

    private static final USBNativeHandlerServices INSTANCE = new USBNativeHandlerServices();

    private ServiceRegistration registerService;

    /**
     * @return Instance of {@link USBNativeHandlerServices}
     */
    public static USBNativeHandlerServices getInstance() {
        return INSTANCE;
    }

    /** */
    private USBNativeHandlerServices() {
        nativeHandler = USBNativeHandler.getInstance();
    }

    /**
     * {@inheritDoc}
     */
    public String getIdentifier() {
        return PORT_PROVIDER_IDENTIFIER;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isTriggered() {
        return nativeHandler.autosenseEnabled();
    }

    /**
     * {@inheritDoc}
     */
    public void release() {
        nativeHandler.disableAutosense();
    }

    /**
     * {@inheritDoc}
     */
    public void trigger() throws PortException {
        if (registerService != null) {
            nativeHandler.enableAutosense();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {

        if (ConfigurationOption.MONITOR.equalsIgnoreCase(newRecord.getName()) && registerService != null) {
            String newValue = newRecord.getValue(ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS);
            String oldValue = null;

            // Old record can be null
            if (oldRecord != null) {
                oldValue = oldRecord.getValue(ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS);
            }

            if (!newValue.equalsIgnoreCase(oldValue)) {
                checkMonitor(service, oldRecord, newRecord);
            }
        }

        if (ConfigurationOption.PORT_USB.equalsIgnoreCase(newRecord.getName())) {
            String newValue = newRecord.getValue(ConfigurationOption.PORT_USB_ENABLED);

            // Old record can be null
            String oldValue = null;
            if (oldRecord != null) {
                oldValue = oldRecord.getValue(ConfigurationOption.PORT_USB_ENABLED);
            }

            if (!newValue.equalsIgnoreCase(oldValue)) {
                checkUSB(service, oldRecord, newRecord);
            }
        }
    }

    private void checkMonitor(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {

        String newValue = newRecord.getValue(ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS);

        boolean enabled = false;
        if ("true".equalsIgnoreCase(newValue)) {
            enabled = true;
        } else if (!"false".equalsIgnoreCase(newValue)) {
            log.error("Unknown boolean value in configuration for acceptEquipments: " + newValue);
            return;
        }

        try {
            if (enabled && !isTriggered()) {
                trigger();
            } else if (!enabled && isTriggered()) {
                release();
            } else {
                return;
            }
            log.info("'" + getIdentifier() + "' port provider has been " + (enabled ? "triggered " : "released")
                + " successfuly");
        } catch (PortException pe) {
            log.error((enabled ? "Enabling " : "Disabling ") + "autosense on '" + getIdentifier() + "' failed!", pe);
        }
    }

    private void checkUSB(IConfigurationService service, IConfigurationRecord oldRecord, IConfigurationRecord newRecord) {

        String newValue = newRecord.getValue(ConfigurationOption.PORT_USB_ENABLED);

        boolean enabled = false;

        if ("true".equalsIgnoreCase(newValue)) {
            enabled = true;
        } else if (!"false".equalsIgnoreCase(newValue)) {
            log.error("Unknown boolean value in configuration for USB enabled: " + newValue);
            return;
        }

        if (enabled) {
            if (registerService == null) {

                // register IPortProvider
                Hashtable<String, String> props = new Hashtable<String, String>();
                props.put(IPortProvider.PORT_PROVIDER_NAME, this.getIdentifier());

                registerService =
                    Activator.getBundleContext().registerService(IPortProvider.class.getName(), this, props);
            }
        } else {
            if (registerService != null) {
                registerService.unregister();
                registerService = null;
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public void simulateUSBDevice(int pVid, int pPid, byte[][] pDataToSent, byte[][] pDataTeRetrieve) {
        nativeHandler.jniInfo("USB communication testing mode..");
        if (nativeHandler.acceptUSBDevice(pVid, pPid) && (registerService != null)) {
            nativeHandler.registerUSB(new USBNativeDevice(pVid, pPid, pDataToSent, pDataTeRetrieve));
        }
    }

    /**
     * {@inheritDoc}
     */
    public void simulateUARTDevice(byte[][] pReadData, byte[][] pWriteData) {
        throw new UnsupportedOperationException();
    }
}
