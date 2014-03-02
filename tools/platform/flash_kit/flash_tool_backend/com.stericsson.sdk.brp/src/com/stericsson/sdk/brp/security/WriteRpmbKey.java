package com.stericsson.sdk.brp.security;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * @author mbocek01
 * 
 */
public class WriteRpmbKey extends AbstractCommand {
    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.SECURITY_WRITE_RPMB_KEY;
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
            PARAMETER_EQUIPMENT_ID, PARAMETER_DEVICE_ID, PARAMETER_COMMERCIAL};
    }
}
