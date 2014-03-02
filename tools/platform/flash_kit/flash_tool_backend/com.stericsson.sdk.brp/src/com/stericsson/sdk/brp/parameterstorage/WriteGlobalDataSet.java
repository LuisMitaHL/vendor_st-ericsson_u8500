package com.stericsson.sdk.brp.parameterstorage;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for writing global data set on an equipment
 * 
 * @author xolabju
 * 
 */
public class WriteGlobalDataSet extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET;
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
            PARAMETER_EQUIPMENT_ID, PARAMETER_STORAGE_ID, PARAMETER_PATH};
    }

}
