package com.stericsson.sdk.equipment;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.brp.util.Constant;
import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.internal.LoaderCommunicationInterface;

/**
 * Extends AbstractPlatform with LoaderCommunicationListener interface implementation. All loader
 * communication based platforms should extend this class.
 * 
 * @author xadazim
 * 
 */
public abstract class AbstractLCPlatform extends AbstractPlatform implements ILoaderCommunicationListener {

    /** */
    private Logger logger = Logger.getLogger(AbstractLCPlatform.class);

    /** */
    public static final String LOAD_LCD_DRIVER_ERROR = "Could not load LCDriver";

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationError(IPort port, String message) {
        ServiceReference[] references = null;

        String filter = "(&(" + IEquipmentTask.PORT_PROPERTY + "=" + port.getPortName() + "))";

        try {
            references = Activator.getBundleContext().getServiceReferences(IEquipment.class.getName(), filter);
            if (references != null) {
                for (ServiceReference reference : references) {
                    IEquipment equipment = (IEquipment) Activator.getBundleContext().getService(reference);
                    if (equipment.getPort().getPortIdentifier().equals(port.getPortIdentifier())) {
                        destroyEquipment(equipment);
                    }
                    Activator.getBundleContext().ungetService(reference);
                }
            }
        } catch (InvalidSyntaxException e) {
            logger.error(e.getMessage(), e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationMessage(String title, String message, boolean logOnly) {
        // Not used
    }

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationProgress(long totalBytes, long transferredBytes) {
        // Not used
    }

    /**
     * Notify all listeners about new booted equipment
     * 
     * @param equipment
     *            equipment
     */
    protected void notifyNewEquipmentBooted(IEquipment equipment) {

        ServiceReference[] references = null;
        try {
            references =
                Activator.getBundleContext().getServiceReferences(IEquipmentBootedListener.class.getName(), null);
            if (references != null) {
                try {
                    for (ServiceReference reference : references) {
                        IEquipmentBootedListener service =
                            (IEquipmentBootedListener) Activator.getBundleContext().getService(reference);
                        if (service != null) {
                            service.newEquipmentBooted(equipment);
                        }
                    }
                } finally {
                    for (ServiceReference reference : references) {
                        Activator.getBundleContext().ungetService(reference);
                    }
                } 
            }
        } catch (InvalidSyntaxException e) {
            logger.error("Cannot get EquipmentBootedListeners : " + e.getMessage());
        }
    }

    /**
     * Check if there are 64bit LCD/LCM drivers in the binary delivery for 64bit OS system
     * @param errorMessage received error message
     * @return the error message
     */
    public String checkIf64bitLcdLcmAreMissing(String errorMessage){
        if(errorMessage.contains(LOAD_LCD_DRIVER_ERROR) && EnvironmentProperties.is64BitVM()){
            boolean exists = LoaderCommunicationInterface.getInstance().check64BitLCDDriversExists() 
                    && LoaderCommunicationInterface.getInstance().check64BitLCMDriversExists();
            logger.info("64 bit LCD and LCM exists in the native folder: " + exists);
            if(!exists){
                errorMessage = Constant.No64BitLcdLcmDriver.getValue();
                logger.error("BOOT task failed! Error message: " + errorMessage);
            }
        }

        return errorMessage;
    }
}
