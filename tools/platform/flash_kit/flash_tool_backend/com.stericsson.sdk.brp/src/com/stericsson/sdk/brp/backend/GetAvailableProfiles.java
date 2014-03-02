package com.stericsson.sdk.brp.backend;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for getting all available profiles
 * 
 * @author xolabju
 * 
 */
public class GetAvailableProfiles extends AbstractCommand {


    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.BACKEND_GET_AVAILABLE_PROFILES;
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
        return new String[] {PARAMETER_WAIT};
    }
}
