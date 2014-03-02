package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;

/**
 * Executor for setting the active profile for a connected equipment
 * 
 * @author xolabju
 * 
 */
public class SetEquipmentProfileExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(SetEquipmentProfileExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        String requiredProfileAlias = getCommand().getValue(AbstractCommand.PARAMETER_PROFILE_NAME);
        // get equipment service
        String port = getCommand().getValue(AbstractCommand.PARAMETER_EQUIPMENT_ID);
        IEquipment equipment = getEquipment(port);
        if (equipment != null) {
            // get profile manager service
            IEquipmentProfileManagerService profileManagerService = getProfileManagerService();
            if (requiredProfileAlias == null || requiredProfileAlias.trim().equals("")) {
                log.debug("Illegal profile alias specified.");
                throw new ExecutionException("Illegal profile alias specified.");
            }

            if (profileManagerService != null) {
                IEquipmentProfile requiredProfile = profileManagerService.getProfile(requiredProfileAlias, true);
                if (requiredProfile == null) {
                    throw new ExecutionException("Cannot set profile " + requiredProfileAlias
                        + " as active! Profile does not exist in available profiles list.");
                }

                equipment.setProfile(requiredProfile);
            } else {
                throw new ExecutionException("Failed to get a ProfileManagerService");
            }
        } else {
            throw new ExecutionException("No connected equipment on port " + port);
        }
        return "Profile " + requiredProfileAlias + " set as active for " + equipment.toString() + ".";
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
