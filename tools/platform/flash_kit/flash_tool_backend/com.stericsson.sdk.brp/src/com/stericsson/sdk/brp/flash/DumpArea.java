package com.stericsson.sdk.brp.flash;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for executing 'dump area' on an equipment
 * 
 * @author xolabju
 * 
 */
public class DumpArea extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.FLASH_DUMP_AREA;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isCancellable() {
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_EQUIPMENT_ID, PARAMETER_AREA_PATH, PARAMETER_OFFSET, PARAMETER_LENGTH, PARAMETER_PATH,
            PARAMETER_SKIP_REDUNDANT_AREA, PARAMETER_INCLUDE_BAD_BLOCKS};
    }

}
