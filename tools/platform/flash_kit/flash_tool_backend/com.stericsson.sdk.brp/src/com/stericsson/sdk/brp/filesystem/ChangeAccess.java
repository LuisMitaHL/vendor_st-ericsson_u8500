package com.stericsson.sdk.brp.filesystem;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for changing access permissions of specified path
 * 
 * @author pkutac01
 * 
 */
public class ChangeAccess extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.FILE_SYSTEM_CHANGE_ACCESS;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_EQUIPMENT_ID, PARAMETER_TARGET_PATH, PARAMETER_ACCESS};
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isCancellable() {
        return false;
    }

}
