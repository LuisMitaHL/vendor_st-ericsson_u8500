package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfile;

/**
 * Executor for getting the active profile for a connected equipment
 * 
 * @author xolabju
 * 
 */
public class GetEquipmentProfileExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(GetEquipmentProfileExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        // get equipment service
        String port = getCommand().getValue(AbstractCommand.PARAMETER_EQUIPMENT_ID);
        IEquipment equipment = getEquipment(port);

        if (equipment != null) {
            IEquipmentProfile profile = equipment.getProfile();
            if (profile != null) {
                return profile.getAlias();
            } else {
                throw new ExecutionException("Failed to get profile from equipment " + equipment.toString());
            }
        } else {
            throw new ExecutionException("No connected equipment on port " + port);
        }

    }
}
