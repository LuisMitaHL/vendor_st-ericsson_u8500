package com.stericsson.sdk.brp.backend;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for getting auto sense status
 * 
 * @author xolabju
 * 
 */
public class GetAutoSenseStatus extends AbstractCommand {

    /** */
    public static final String AUTO_SENSE_STATUS_ON = "ON";

    /** */
    public static final String AUTO_SENSE_STATUS_OFF = "OFF";


    /**
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.BACKEND_GET_AUTO_SENSE_STATUS;
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
