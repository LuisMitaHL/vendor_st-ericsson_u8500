package com.stericsson.sdk.brp.system;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for authenticating using control keys
 * 
 * @author xolabju
 * 
 */
public class AuthenticateControlKeys extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS;
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
            PARAMETER_EQUIPMENT_ID, PARAMETER_CONTROL_KEYS, PARAMETER_PERMANENT};
    }
}
