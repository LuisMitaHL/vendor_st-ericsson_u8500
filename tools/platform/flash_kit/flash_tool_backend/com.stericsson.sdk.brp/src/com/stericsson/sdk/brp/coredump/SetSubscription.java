package com.stericsson.sdk.brp.coredump;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * 
 * Remote command for setting up a subscription.
 * 
 * @author qkarhed
 */
public class SetSubscription extends AbstractCommand {

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
        return CommandName.COREDUMP_SET_SUBSCRIPTION;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_COREDUMP_SUBSCRIBER_IP, PARAMETER_COREDUMP_SUBSCRIBER_PORT, PARAMETER_COREDUMP_SUBSCRIPTION_TYPE};
    }
}
