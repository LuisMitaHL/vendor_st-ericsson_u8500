package com.stericsson.sdk.brp.coredump;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for listing dump files
 * 
 * @author esrimpa
 * 
 */
public class ListFile extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.COREDUMP_LIST_FILE;
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
            PARAMETER_EQUIPMENT_ID};
    }
}
