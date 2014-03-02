/**
 * 
 */
package com.stericsson.sdk.backend.remote.executor.backend;

import java.util.Hashtable;
import java.util.Iterator;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;

/**
 * @author xhelciz
 * 
 */
public class GetAvailableSecurityPropertiesFromProfileExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(GetAvailableSecurityPropertiesFromProfileExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        // get active profile alias
        String activeProfileAlias;
        String resultMessage = "";
        IEquipmentProfile activeProfile = null;

        GetActiveProfileExecutor executor = new GetActiveProfileExecutor();
        try {
            AbstractCommand command = CommandFactory.createCommand(CommandName.BACKEND_GET_ACTIVE_PROFILE.name());
            executor.setCommand(command);
            activeProfileAlias = executor.execute();

            // get profile manager service
            IEquipmentProfileManagerService profileManagerService = getProfileManagerService();

            if (profileManagerService != null) {
                activeProfile = profileManagerService.getProfile(activeProfileAlias, true);
            }
            String propertiesList = getPropertiesList(activeProfile);
            if (propertiesList != null) {
                resultMessage = propertiesList;
            } else {
                // fail
                resultMessage = "No properties found.";
            }
        } catch (com.stericsson.sdk.brp.InvalidSyntaxException e) {
            throw new ExecutionException(e.getMessage());
        }

        return resultMessage;
    }

    private String getPropertiesList(IEquipmentProfile activeProfile) {
        StringBuffer buffer = new StringBuffer();

        if (activeProfile == null) {
            return "";
        }
        Hashtable<String, EquipmentProperty> securityProperties = activeProfile.getSupportedSecurityProperties();
        if (!securityProperties.isEmpty()) {
            // propertyList = PROPERTY_NAME + PROPERTY_ID + PROPERTY_READ + PROPERTY_WRITE;
            Iterator<EquipmentProperty> it = securityProperties.values().iterator();
            while (it.hasNext()) {
                EquipmentProperty property = it.next();
                buffer.append(property.getPropertyName() + "<newcolumn>" + property.getPropertyId() + "<newcolumn>"
                    + property.isReadable() + "<newcolumn>" + property.isWritable() + AbstractCommand.DELIMITER);

            }
        }
        return buffer.toString();
    }

    private IEquipmentProfileManagerService getProfileManagerService() throws ExecutionException {
        ServiceReference[] references = null;

        try {
            references =
                Activator.getBundleContext().getAllServiceReferences(IEquipmentProfileManagerService.class.getName(),
                    "(type=profile)");
        } catch (InvalidSyntaxException e) {
            log.error(e.getMessage());
        }

        if (references == null || references.length == 0) {
            log.error("Cannot find IEquipmentProfileManagerService.");
            throw new ExecutionException("No active profile.");
        }
        IEquipmentProfileManagerService profileManagerService =
            (IEquipmentProfileManagerService) Activator.getBundleContext().getService(references[0]);

        return profileManagerService;
    }

}
