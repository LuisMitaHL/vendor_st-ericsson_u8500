package com.stericsson.sdk.backend.remote.executor.system;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.LCCommandExecutor;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutorFactory;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.ICommandExecutor;
import com.stericsson.sdk.brp.IExecutionHandler;
import com.stericsson.sdk.brp.InvalidSyntaxException;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortUtilities;

/**
 * Remote command executor for "reboot equipment" command.
 * 
 * @author pkutac01
 * 
 */
public class RebootEquipmentExecutor extends LCCommandExecutor {

    private static Logger logger = Logger.getLogger(RebootEquipmentExecutor.class);

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        String resultMessage = super.execute();

        IPort port = getEquipment().getPort();
        if (!PortUtilities.isUSBPort(port)) {
            final String portName = port.getPortName();
            final String commandString =
                CommandName.BACKEND_TRIGGER_UART_PORT.name() + AbstractCommand.DELIMITER + portName;
            try {
                IExecutionHandler handler = new IExecutionHandler() {
                    public void fatalError(AbstractCommand command, String errorMessage) {
                        logger.error("Triggering UART port " + portName + " produced an error: " + errorMessage);
                    }

                    public void done(ICommandExecutor executor) {
                        logger.debug("Triggering UART port " + portName + " was successful.");
                    }
                };

                ICommandExecutor executor = ServerCommandExecutorFactory.createCommandExecutor(commandString, handler);
                if (executor == null) {
                    throw new ExecutionException("Created executor is null.");
                }
                executor.execute();
            } catch (InvalidSyntaxException e) {
                throw new ExecutionException(e);
            }
        }

        return resultMessage;
    }

}
