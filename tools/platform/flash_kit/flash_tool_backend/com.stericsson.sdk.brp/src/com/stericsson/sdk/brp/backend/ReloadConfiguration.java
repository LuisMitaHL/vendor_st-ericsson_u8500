package com.stericsson.sdk.brp.backend;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for reloading the backend configuration
 * 
 * @author xolabju
 * 
 */
public class ReloadConfiguration extends AbstractCommand {

    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.BACKEND_RELOAD_CONFIGURATION;
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
        return null;
    }
}
