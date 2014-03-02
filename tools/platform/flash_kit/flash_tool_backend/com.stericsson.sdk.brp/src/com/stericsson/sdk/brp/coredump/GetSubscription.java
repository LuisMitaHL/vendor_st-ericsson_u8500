package com.stericsson.sdk.brp.coredump;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for getting subscriptions.
 * 
 * @author qkarhed
 */
public class GetSubscription extends AbstractCommand {

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
    public CommandName getCommandName() {
        return CommandName.COREDUMP_GET_SUBSCRIPTION;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_COREDUMP_SUBSCRIBER_IP, PARAMETER_COREDUMP_SUBSCRIBER_PORT};
    }
}
