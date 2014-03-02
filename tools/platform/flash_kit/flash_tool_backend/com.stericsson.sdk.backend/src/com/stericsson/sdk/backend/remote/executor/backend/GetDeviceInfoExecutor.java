package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.AbstractEquipment;

/**
 * @author mbocek01
 * 
 */
public class GetDeviceInfoExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(TriggerUARTPortExecutor.class);

    /**
     * @return device info
     * @throws ExecutionException .
     */
    @Override
    public String execute() throws ExecutionException {
        String returnString = "";
        String equipmentId = getCommand().getValue(AbstractCommand.PARAMETER_EQUIPMENT_ID);
        AbstractEquipment equipment = (AbstractEquipment) getEquipment(equipmentId);
        if (equipment == null) {
            return "No equipment connected on port " + equipmentId;
        }
        returnString = equipment.getInfo();
        if (returnString.equalsIgnoreCase("")) {
            returnString = "No equipment connected on port " + equipmentId;
        }
        return returnString;
    }

}
