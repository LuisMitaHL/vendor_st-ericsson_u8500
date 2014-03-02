package com.stericsson.sdk.backend.remote.executor.backend;

import java.io.IOException;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentException;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;

/**
 * Executor for getting all available profiles
 * 
 * @author xolabju
 * 
 */
public class GetAvailableProfilesExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(EnableAutoSenseExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {

        IEquipmentProfileManagerService profileManagerService = getProfileManagerService();

        if (profileManagerService == null) {
            throw new ExecutionException("Failed to get ProfileManagerService");
        }

        // wait for available profiles to load
        if (getCommand().getValue(AbstractCommand.PARAMETER_WAIT).equalsIgnoreCase("y")
            || (getCommand().getValue(AbstractCommand.PARAMETER_WAIT).equalsIgnoreCase("yes"))) {
            // provide user with loading progress
            checkLoading(profileManagerService);
        }

        String[] availableProfileNames;
        try {
            availableProfileNames = profileManagerService.getAllAvailableProfilesNames();
        } catch (EquipmentException e) {
            throw new ExecutionException(e.getMessage());
        }
        if (availableProfileNames == null || availableProfileNames.length == 0) {
            throw new ExecutionException("No available profiles");
        }

        return prepareString(availableProfileNames);
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
            throw new ExecutionException("No available profiles.");
        }

        IEquipmentProfileManagerService profileManagerService =
            (IEquipmentProfileManagerService) Activator.getBundleContext().getService(references[0]);
        return profileManagerService;
    }

    /**
     * 
     * @param profileManagerService
     *            profile manager service
     * @throws ExecutionException
     *             on errors
     */
    public void checkLoading(IEquipmentProfileManagerService profileManagerService) throws ExecutionException {
        int oldPercent = -1;
        int newPercent = 0;

        if (profileManagerService == null) {
            throw new ExecutionException("ProfileManagerService is null");
        }
        while (profileManagerService.isLoading()) {
            newPercent = profileManagerService.getPercentageOfLoadedProfiles();
            if (newPercent != oldPercent) {
                try {
                    progressPercent(newPercent);
                } catch (IOException e) {
                    throw new ExecutionException(e.getMessage());
                }
            }
            oldPercent = newPercent;
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private String prepareString(String[] availableProfileNames) {
        String profileName;
        StringBuffer buffer = new StringBuffer();
        int index = 1;
        for (int i = 0; i < availableProfileNames.length; i++) {
            profileName = availableProfileNames[i];
            buffer.append(profileName);
            if (index < availableProfileNames.length) {
                buffer.append(AbstractCommand.DELIMITER);
            }
            index++;
        }
        return buffer.toString();
    }
}
