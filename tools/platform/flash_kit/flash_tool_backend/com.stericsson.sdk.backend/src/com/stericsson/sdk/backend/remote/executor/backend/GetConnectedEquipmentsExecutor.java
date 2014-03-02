package com.stericsson.sdk.backend.remote.executor.backend;

import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IPlatform;

/**
 * Executor for getting all available profiles
 * 
 * @author xolabju
 * 
 */
public class GetConnectedEquipmentsExecutor extends ServerCommandExecutor {

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        String returnString = "";
        ServiceReference[] serviceReferences = null;
        try {
            serviceReferences = Activator.getBundleContext().getServiceReferences(IEquipment.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }
        if (serviceReferences != null) {
            for (int i = 0; i < serviceReferences.length; i++) {
                IEquipment equipment = (IEquipment) Activator.getBundleContext().getService(serviceReferences[i]);
                returnString +=
                    equipment.getProperty(IPlatform.PLATFORM_PROPERTY).getPropertyValue() + AbstractCommand.DELIMITER;
                returnString += equipment.getProperty(IPlatform.PORT_PROPERTY).getPropertyValue() + AbstractCommand.DELIMITER;
                returnString += equipment.getProfile().getAlias() + AbstractCommand.DELIMITER;
                returnString += equipment.getStatus().getState().name() + AbstractCommand.DELIMITER;
                returnString +=
                    equipment.getStatus().getStateMessage().trim().equals("") ? "---" + AbstractCommand.DELIMITER : equipment
                        .getStatus().getStateMessage()
                        + AbstractCommand.DELIMITER;
            }
        }

        return returnString;
    }
}
