package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;

/**
 * Executor for setting the active profile
 * 
 * @author xolabju
 * 
 */
public class SetActiveProfileExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(SetActiveProfileExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {

        // get profile manager service
        IEquipmentProfileManagerService profileManagerService = getProfileManagerService();
        String requiredProfileAlias = getCommand().getValue(AbstractCommand.PARAMETER_PROFILE_NAME);
        if (requiredProfileAlias == null || requiredProfileAlias.trim().equals("")) {
            log.debug("Illegal active profile alias specified.");
            throw new ExecutionException("Illegal active profile alias specified.");
        }

        if (profileManagerService != null) {
            IEquipmentProfile requiredProfile = profileManagerService.getProfile(requiredProfileAlias, true);
            if (requiredProfile == null) {
                throw new ExecutionException("Cannot set profile " + requiredProfileAlias
                    + " as active! Profile does not exist in available profiles list.");
            }

            setActiveProfile(requiredProfileAlias);
        }
        return "Profile " + requiredProfileAlias + " is active.";
    }

    private void setActiveProfile(String pRequiredProfileAlias) throws ExecutionException {
        IBackendService backendService = getBackendService();
        if (backendService != null) {
            backendService.setActiveProfile(pRequiredProfileAlias);
        }
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

    private IBackendService getBackendService() throws ExecutionException {
        ServiceReference[] references = null;
        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            log.error(e.getMessage());
        }

        if (references == null || references.length == 0) {
            log.error("Cannot find IBackendService.");
            throw new ExecutionException("No active profile.");
        }
        IBackendService backendService = (IBackendService) Activator.getBundleContext().getService(references[0]);
        return backendService;
    }
}
