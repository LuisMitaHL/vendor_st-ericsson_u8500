package com.stericsson.sdk.brp.flash;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * 
 * @author mbocek01
 * 
 */
public class SetEnhancedArea extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.FLASH_SET_ENHANCED_AREA;
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
