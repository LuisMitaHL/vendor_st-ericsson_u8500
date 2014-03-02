package com.stericsson.sdk.equipment.internal;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.Activator;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;
import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortDataListener;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomlju
 */
public class PlatformPortReceiver implements IPortReceiver, IPortDataListener {

    private static Logger logger = Logger.getLogger(PlatformPortReceiver.class);

    /**
     * {@inheritDoc}
     */
    public void deliverPort(IPort port, PortDeliveryState state) throws PortException {
        deliverPort(port, state, true);
    }

    /**
     * {@inheritDoc}
     */
    public void deliverPort(IPort port, PortDeliveryState state, boolean bootME) throws PortException {

        logger.info("New port delivered: " + port.getPortName());

        // Lookup profile

        IEquipmentProfile profile = lookupProfile(findActiveProfileAlias());

        if (profile == null) {
            throw new PortException("Cannot deliver port with null profile.");
        }

        logger.info("Using profile: " + profile.getAlias());

        // Get platform service for specified profile

        ServiceReference[] references = null;

        try {

            references =
                Activator.getBundleContext().getServiceReferences(IPlatform.class.getName(),
                    "(&" + profile.getPlatformFilter() + "(state=" + state.name() + "))");

            if (references == null) {
                throw new PortException("No platform service found matching the filter " + profile.getPlatformFilter());
            }
        } catch (InvalidSyntaxException e) {
            throw new PortException("Filter expression syntax error", e);
        }

        // Only pick the first reference in array. If more than one matches the filter
        // expression must be improved further.

        IPlatform platform = (IPlatform) Activator.getBundleContext().getService(references[0]);

        // Open port

        if (!port.isOpen()) {
            port.open();
        }

        // Create equipment

        platform.createEquipment(port, profile, bootME);

        // Release the service references

        Activator.getBundleContext().ungetService(references[0]);
    }

    /**
     * {@inheritDoc}
     */
    public void closePort(IPort port) throws PortException {
        ServiceReference[] platformReferences = null;
        try {
            platformReferences = Activator.getBundleContext().getServiceReferences(IPlatform.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            logger.error("Failed to get platform service references", e);
        }

        if (platformReferences != null) {
            ServiceReference[] equipmentReferences = null;
            try {
                // String filter = "(port=" + port.getIdentifier() + ")";
                String filter = "(port=*)";
                equipmentReferences =
                    Activator.getBundleContext().getServiceReferences(IEquipment.class.getName(), filter);

                if (equipmentReferences != null) {
                    for (int i = 0; i < equipmentReferences.length; i++) {
                        IEquipment equipment =
                            (IEquipment) Activator.getBundleContext().getService(equipmentReferences[i]);
                        if (port.getPortName().contentEquals(equipment.getProperty("port").getPropertyValue())) {
                            logger.info("Trying to unregister equipment " + equipment);
                            for (ServiceReference ref : platformReferences) {
                                IPlatform platform = (IPlatform) Activator.getBundleContext().getService(ref);
                                platform.destroyEquipment(equipment);
                            }
                        }
                    }
                }
            } catch (InvalidSyntaxException e) {
                logger.error("Failed to get equipment service references", e);
            }

        }

    }

    /**
     * {@inheritDoc}
     */
    public void bytesRead(IPort port, byte[] buffer, int offset, int count) {
        StringBuffer message = new StringBuffer();
        message.append(count + " bytes read from port: " + port.getPortName());
        if (count > 0) {
            if (count > 128) {
                message.append("\n" + HexUtilities.toHexString(buffer, offset, 128, 32, true));
                message.append("\n... (" + (count - 128) + " more bytes)");
            } else {
                message.append("\n" + HexUtilities.toHexString(buffer, offset, count, 32, true));
            }
        }
        logger.debug(message);
    }

    /**
     * {@inheritDoc}
     */
    public void bytesWritten(IPort port, byte[] buffer, int offset, int count) {
        StringBuffer message = new StringBuffer();
        message.append(count + " bytes written to port: " + port.getPortName());
        if (count > 0) {
            if (count > 128) {
                message.append("\n" + HexUtilities.toHexString(buffer, offset, 128, 32, true));
                message.append("\n... (" + (count - 128) + " more bytes)");
            } else {
                message.append("\n" + HexUtilities.toHexString(buffer, offset, count, 32, true));
            }
        }
        logger.debug(message);
    }

    private IEquipmentProfile lookupProfile(String profileAlias) {
        IEquipmentProfile profile = null;
        ServiceReference reference = null;

        reference = Activator.getBundleContext().getServiceReference(IEquipmentProfileManagerService.class.getName());

        IEquipmentProfileManagerService service =
            (IEquipmentProfileManagerService) Activator.getBundleContext().getService(reference);

        profile = service.getProfile(profileAlias, true);

        Activator.getBundleContext().ungetService(reference);

        return profile;
    }

    private String findActiveProfileAlias() {
        String profileAlias = null;
        ServiceReference[] references = null;

        String filter = ("(&(type=backend)(format=mesp))");

        try {
            references =
                Activator.getBundleContext().getServiceReferences(IConfigurationService.class.getName(), filter);
            if ((references != null) && (references.length > 0)) {
                IConfigurationService configurationService =
                    (IConfigurationService) Activator.getBundleContext().getService(references[0]);
                IConfigurationRecord[] records = configurationService.getRecords(new String[] {
                    "ActiveProfile"});
                if (records.length > 0) {
                    profileAlias = records[0].getValue("alias");
                }

                Activator.getBundleContext().ungetService(references[0]);
            }
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        return profileAlias;
    }

}
