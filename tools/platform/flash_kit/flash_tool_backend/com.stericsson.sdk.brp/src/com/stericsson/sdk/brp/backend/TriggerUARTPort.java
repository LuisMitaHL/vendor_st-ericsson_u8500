package com.stericsson.sdk.brp.backend;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;

/**
 * Remote command for triggering UART port.
 * 
 * @author pkutac01
 * 
 */
public class TriggerUARTPort extends AbstractCommand {

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public CommandName getCommandName() {
        return CommandName.BACKEND_TRIGGER_UART_PORT;
    }

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public String[] getParameterNames() {
        return new String[] {
            PARAMETER_PORT_NAME};
    }

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public boolean isCancellable() {
        return false;
    }

}
