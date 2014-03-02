package com.stericsson.sdk.brp.flash;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for executing 'erase area' on an equipment
 * 
 * @author xolabju
 * 
 */
public class EraseArea extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.FLASH_ERASE_AREA;
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
            PARAMETER_EQUIPMENT_ID, PARAMETER_AREA_PATH, PARAMETER_OFFSET, PARAMETER_LENGTH};
    }

}
