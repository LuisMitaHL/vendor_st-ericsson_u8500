package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;

/**
 * Executor for getting the active profile
 * 
 * @author xolabju
 * 
 */
public class GetActiveProfileExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(GetActiveProfileExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        // get configuration service
        IEquipmentProfile activeProfile = null;
        IConfigurationService configurationService = getConfigurationService();

        // get active profile from mesp file
        IConfigurationRecord activeProfileRecord =
            configurationService == null ? null : configurationService.getRecord(ConfigurationOption.ACTIVE_PROFILE);
        if (activeProfileRecord == null) {
            log.debug("No active profile specified in configuration.");
            throw new ExecutionException("No active profile.");
        }

        // get active profile alias
        String activeProfileRecordAlias = activeProfileRecord.getValue(ConfigurationOption.PROFILE_ALIAS);
        if (activeProfileRecordAlias == null || activeProfileRecordAlias.trim().equals("")) {
            log.debug("Illegal active profile alias specified.");
            throw new ExecutionException("No active profile. Illegal active profile alias specified.");
        }

        // get profile manager service
        IEquipmentProfileManagerService profileManagerService = getProfileManagerService();

        if (profileManagerService != null) {
            activeProfile = profileManagerService.getProfile(activeProfileRecordAlias, true);
        }

        if (activeProfile == null) {
            log.debug("Active profile specified in configuration is not in available profiles list.");
            throw new ExecutionException("No active profile.");
        }
        return activeProfile.getAlias();
    }

    /**
     * @return IConfigurationService
     * @throws ExecutionException ExecutionException
     */
    protected IConfigurationService getConfigurationService() throws ExecutionException {
        ServiceReference[] references = null;
        try {
            references =
                Activator.getBundleContext().getAllServiceReferences(IConfigurationService.class.getName(),
                    "(type=backend)");
        } catch (InvalidSyntaxException e) {
            log.error(e.getMessage());
        }

        if (references == null || references.length == 0) {
            log.error("Cannot find IConfigurationService.");
            throw new ExecutionException("No active profile.");
        }

        IConfigurationService configurationService =
            (IConfigurationService) Activator.getBundleContext().getService(references[0]);

        return configurationService;
    }

    /**
     * @return IEquipmentProfileManagerService
     * @throws ExecutionException ExecutionException
     */
    protected IEquipmentProfileManagerService getProfileManagerService() throws ExecutionException {
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
