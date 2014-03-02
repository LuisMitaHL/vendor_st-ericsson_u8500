package com.stericsson.sdk.brp.parameterstorage;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for reading a global data unit on an equipment
 * 
 * @author xolabju
 * 
 */
public class ReadGlobalDataUnit extends AbstractCommand {


    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isCancellable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_EQUIPMENT_ID, PARAMETER_STORAGE_ID, PARAMETER_UNIT_ID};
    }

}
