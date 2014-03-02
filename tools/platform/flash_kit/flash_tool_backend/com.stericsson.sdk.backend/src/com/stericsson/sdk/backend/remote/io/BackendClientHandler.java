package com.stericsson.sdk.backend.remote.io;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutorFactory;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ICommandExecutor;
import com.stericsson.sdk.brp.IExecutionHandler;
import com.stericsson.sdk.brp.InvalidSyntaxException;
import com.stericsson.sdk.brp.ServerResponse;

/**
 * Class that handles the communication with the client
 * 
 * @author xolabju
 * 
 */
public class BackendClientHandler implements Runnable, IExecutionHandler {

    private static Logger logger = Logger.getLogger(BackendClientHandler.class);

    private IBackendServerConnection client;

    private boolean isRunning;

    private HashMap<String, ServerCommandExecutor> longRunningCommands;

    /**
     * 
     * @param bc
     *            the backend client
     */
    public BackendClientHandler(IBackendServerConnection bc) {
        longRunningCommands = new HashMap<String, ServerCommandExecutor>();
        client = bc;
    }

    /**
     * {@inheritDoc}
     */
    public void run() {
        isRunning = true;
        logger.info("Started communication with " + client.getAddress());
        while (isRunning) {
            try {
                String command = client.readLine();
                if (command != null && !command.trim().equals("")) {
                    handleCommand(command);
                } else if (command == null) {
                    disconnect();
                }

            } catch (IOException ioe) {
                logger.error("Error when reading command input: " + ioe.getMessage());

                // cancel current commands
                for (Iterator<String> it = longRunningCommands.keySet().iterator(); it.hasNext();) {
                    longRunningCommands.get(it.next()).cancel();
                }

                disconnect();

            }
        }

    }

    private void handleCommand(String command) {

        ServerCommandExecutor executor = null;
        try {
            executor = (ServerCommandExecutor) ServerCommandExecutorFactory.createCommandExecutor(command, this);
            if (executor != null) {
                String consoleOutput =
                    "Received BRP command: " + command + "\n"
                        + CommandName.getCLISyntax(executor.getCommand().getCommandName().name());
                String[] parameterNames = executor.getCommand().getParameterNames();
                if (parameterNames != null) {
                    StringBuffer consoleOutputBuffer = new StringBuffer();
                    for (String parameterName : parameterNames) {
                        consoleOutputBuffer.append("  -");
                        consoleOutputBuffer.append(parameterName);
                        consoleOutputBuffer.append(" ");
                        consoleOutputBuffer.append(executor.getCommand().getValue(parameterName));
                    }
                    consoleOutput += consoleOutputBuffer.toString();
                }
                logger.debug(consoleOutput);

                if (executor.getCommand().isCancelCommand()) {
                    try {
                        handleCancel(executor);
                    } catch (IOException ioe) {
                        logger.error("Failed to handle cancel command from " + client.getAddress()
                            + ". Disconnecting. " + ioe.getMessage());
                        disconnect();
                    }
                } else {
                    if (executor.getCommand().isCancellable()) {
                        addLongRunningCommand(executor);
                    }
                    new Thread(executor, "CommandExecutor @ " + client.getAddress()).start();
                }
            } else {
                try {
                    writeNAKMessage(command);
                } catch (IOException ioe) {
                    logger.error("Failed to write NAK response for command " + command + " to " + client.getAddress()
                        + ". Disconnecting. " + ioe.getMessage());
                }
            }
        } catch (InvalidSyntaxException ise) {
            try {
                writeInvalidSyntaxMessage(command, ise.getMessage());
            } catch (IOException ioe) {
                logger.error("Failed to write response message to " + client.getAddress() + ". Disconnecting. "
                    + ioe.getMessage());
                disconnect();
            }
        }

    }

    private void addLongRunningCommand(ServerCommandExecutor executor) {
        longRunningCommands.put(executor.getCommand().getCommandString(), executor);
    }

    private void handleCancel(ServerCommandExecutor executor) throws IOException {
        // remove trailing cancel
        int cancelLength = (AbstractCommand.DELIMITER + AbstractCommand.CANCEL).length();
        String cmdString = executor.getCommand().getCommandString();
        String cmdWithoutCancel = cmdString.substring(0, (cmdString.length() - cancelLength));
        ServerCommandExecutor runningExecutor = longRunningCommands.get(cmdWithoutCancel);
        if (runningExecutor == null) {
            writeErrorMessage(executor.getCommand().getCommandString(), "Process not found");
        } else {
            runningExecutor.cancel();
        }
    }

    /**
     * Disconnects the client
     */
    public void disconnect() {
        synchronized (this) {
            if (isRunning) {
                isRunning = false;
                logger.info("Closing connection to " + client.getAddress());
                client.close();
            }

        }
    }

    /**
     * Writes a message to the client
     * 
     * @param command
     *            the complete command, including supplied parameters
     * @param bw
     *            the writer to use
     * @throws IOException
     *             on errors
     */
    private void writeNAKMessage(String command) throws IOException {
        writeMessage(command + AbstractCommand.DELIMITER + ServerResponse.NAK);
    }

    /**
     * Writes an invalid syntax message to the client
     * 
     * @param command
     *            the complete command, including supplied parameters
     * @param commandDescription
     *            the correct command syntax
     * @throws IOException
     *             on errors
     */
    private void writeInvalidSyntaxMessage(String command, String error) throws IOException {
        writeMessage(command + AbstractCommand.DELIMITER + ServerResponse.ERROR + AbstractCommand.DELIMITER + error);
    }

    /**
     * Writes a message to the client
     * 
     * @param message
     *            the message
     * @throws IOException
     *             if the writing of the message fails
     */
    public void writeMessage(String message) throws IOException {
        message = escapeSequence(message);
        client.write(message);
    }

    private String escapeSequence(String pMessage) {
        if (pMessage != null && !pMessage.trim().equals("")) {
            return pMessage.replace("\r\n", "<newLine>").replace("\n", "<newLine>");
        }
        return pMessage;
    }

    /**
     * Writes an error message for a specified command
     * 
     * @param command
     *            the complete command, including supplied parameters
     * @param error
     *            the additonal message to return together with the error response
     * @throws IOException
     *             on errors
     */
    public void writeErrorMessage(String command, String error) throws IOException {
        writeMessage(command + AbstractCommand.DELIMITER + ServerResponse.ERROR + AbstractCommand.DELIMITER + error);
    }

    /**
     * {@inheritDoc}
     */
    public void done(ICommandExecutor executor) {
        longRunningCommands.remove(executor.getCommand().getCommandString());
    }

    /**
     * {@inheritDoc}
     */
    public void fatalError(AbstractCommand command, String errorMessage) {
        if (command != null) {
            logger.error("Error when executing command " + command.getCommandString() + " - " + errorMessage);
        } else {
            logger.error("Error when executing unknown command " + errorMessage);
        }
        disconnect();
    }

    /**
     * 
     * @return true if the client handler is running (i.e. reading input), else false
     */
    public boolean isRunning() {
        return isRunning;
    }
}
