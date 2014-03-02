package com.stericsson.sdk.backend.remote.test;

import java.io.IOException;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.backend.remote.io.BackendClientHandler;
import com.stericsson.sdk.backend.remote.io.IBackendServerConnection;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * 
 * @author xolabju
 * 
 */
public class BackendClientHandlerTest extends TestCase {

    /** */
    static final String ARG = "test";

    /**
     * 
     */
    @Test
    public void testClientHandler() {
        // create a client that generates execution lines for all command and finally sends null to
        // make the client handler stop
        IBackendServerConnection client = new IBackendServerConnection() {

            int numCommands = CommandName.values().length;

            int currentNumber = 0;

            String cancelCommand = null;

            public void write(String line) throws IOException {
                assertNotNull(line);
            }

            public String readLine() throws IOException {
                if (cancelCommand != null) {
                    String temp = cancelCommand;
                    cancelCommand = null;
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    return temp;
                } else if (currentNumber < numCommands) {
                    CommandName cmdName = CommandName.values()[currentNumber];
                    if (cmdName == CommandName.BACKEND_SHUTDOWN_BACKEND) {
                        System.out.println("Skipping shutdown backend command in test execution");
                        if (++currentNumber < numCommands) {
                            cmdName = CommandName.values()[currentNumber];
                        } else {
                            return null;
                        }
                    }

                    AbstractCommand cmd = CommandFactory.createCommand(cmdName);
                    try {
                        if (cmd == null) {
                            throw new Exception("Command is null");
                        }
                        cmd.setCommand(createCommand(cmdName.name(), cmd.getParameterNames() == null ? 0 : cmd
                            .getParameterNames().length));
                        setPort(cmd);
                        if (cmd.isCancellable()) {
                            cancelCommand =
                                CommandFactory.createCancelCommand(cmd.getCommandString()).getCommandString();
                        }
                    } catch (Exception e) {
                        fail(e.getMessage());
                    }

                    currentNumber++;
                    if (cmd != null) {
                        return cmd.getCommandString();
                    } else {
                        fail("Command is null");
                        return null;
                    }

                } else {
                    return null;
                }
            }

            private void setPort(AbstractCommand cmd) {
                String[] parameterNames = cmd.getParameterNames();
                if (parameterNames != null) {
                    for (String param : parameterNames) {
                        if (AbstractCommand.PARAMETER_EQUIPMENT_ID.equals(param)) {
                            try {
                                cmd.setValue(AbstractCommand.PARAMETER_EQUIPMENT_ID, "PortA");
                                return;
                            } catch (InvalidSyntaxException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }
            }

            public String getAddress() {
                return "127.0.0.1:12345";
            }

            public void close() {

            }
        };

        BackendClientHandler handler = new BackendClientHandler(client);
        Thread t = new Thread(handler);
        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        assertFalse(t.isAlive());
        assertFalse(handler.isRunning());

    }

    /**
     * 
     */
    @Test
    public void testClientHandlerWithInvalidParams() {
        // create a client that generates execution lines for all command and finally sends null to
        // make the client handler stop
        IBackendServerConnection client = new IBackendServerConnection() {

            int numCommands = CommandName.values().length;

            int currentNumber = 0;

            String cancelCommand = null;

            public void write(String line) throws IOException {
                assertNotNull(line);
            }

            public String readLine() throws IOException {
                if (cancelCommand != null) {
                    String temp = cancelCommand;
                    cancelCommand = null;
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    return temp;
                } else if (currentNumber < numCommands) {
                    CommandName cmdName = CommandName.values()[currentNumber];
                    AbstractCommand cmd = CommandFactory.createCommand(cmdName);
                    try {
                        if (cmd == null) {
                            throw new Exception("Command is null");
                        }
                        cmd.setCommand(createInvalidCommand(cmdName.name(), cmd.getParameterNames() == null ? 0 : cmd
                            .getParameterNames().length));
                        if (cmd.isCancellable()) {
                            cancelCommand =
                                CommandFactory.createCancelCommand(cmd.getCommandString()).getCommandString();
                        }
                    } catch (Exception e) {
                        fail(e.getMessage());
                    }

                    currentNumber++;
                    if (cmd != null) {
                        return cmd.getCommandString();
                    } else {
                        fail("Command is null");
                        return null;
                    }

                } else {
                    return null;
                }
            }

            public String getAddress() {
                return "127.0.0.1:12345";
            }

            public void close() {

            }
        };

        BackendClientHandler handler = new BackendClientHandler(client);
        Thread t = new Thread(handler);
        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        assertFalse(handler.isRunning());

    }

    /**
     * 
     */
    @Test
    public void testClientFatalError() {
        // create a client that generates execution lines for all command and finally sends null to
        // make the client handler stop
        IBackendServerConnection client = new IBackendServerConnection() {

            String[] commands = new String[] {
                "invalidCommand", "FLASH_PROCESS_FILE;arg", "FLASH_PROCESS_FILE;arg;arg;CANCEL"};

            int numCommands = commands.length;

            int currentNumber = 0;

            public void write(String line) throws IOException {
                assertNotNull(line);
            }

            public String readLine() throws IOException {
                if (currentNumber < numCommands) {
                    String command = commands[currentNumber];
                    currentNumber++;
                    return command;

                } else {
                    currentNumber = 0;
                    return readLine();
                }
            }

            public String getAddress() {
                return "127.0.0.1:12345";
            }

            public void close() {

            }
        };

        BackendClientHandler handler = new BackendClientHandler(client);
        Thread t = new Thread(handler);
        t.start();

        try {
            Thread.sleep(2000);
            assertTrue(handler.isRunning());
            Thread.sleep(2000);
            handler.fatalError(CommandFactory.createCommand(CommandName.BACKEND_DISABLE_AUTO_INITIALIZE), "test error");
            assertFalse(handler.isRunning());
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

    }

    /**
     * 
     */
    @Test
    public void testRunClientHandlerReadError() {
        // create a client that generates execution lines for all command and finally sends null to
        // make the client handler stop
        IBackendServerConnection client = new IBackendServerConnection() {

            String[] commands = new String[] {
                "invalidCommand", "FLASH_PROCESS_FILE;arg", "FLASH_PROCESS_FILE;arg;arg;CANCEL"};

            int numCommands = commands.length;

            int currentNumber = 0;

            public void write(String line) throws IOException {
                assertNotNull(line);
            }

            public String readLine() throws IOException {
                if (currentNumber < numCommands) {
                    String command = commands[currentNumber];
                    currentNumber++;
                    return command;

                } else {
                    throw new IOException("test error");
                }
            }

            public String getAddress() {
                return "127.0.0.1:12345";
            }

            public void close() {

            }
        };

        BackendClientHandler handler = new BackendClientHandler(client);
        Thread t = new Thread(handler);
        t.start();

        try {
            Thread.sleep(1000);
            assertFalse(handler.isRunning());
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private String[] createCommand(String cmdName, int numArgs) {
        String[] args = new String[numArgs + 1];
        args[0] = cmdName;
        for (int i = 0; i < numArgs; i++) {
            // "test" as equipment-id
            if (i == 0) {
                args[i + 1] = ARG;
            } else {
                if (cmdName.contentEquals(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name())) {
                    // name=value for set properties command
                    args[i + 1] = "name=value";
                } else {
                    // a number for al the other
                    args[i + 1] = "0x" + i;
                }
            }
        }
        return args;
    }

    private String[] createInvalidCommand(String cmdName, int numArgs) {
        String[] args = new String[numArgs + 1];
        args[0] = cmdName;
        for (int i = 0; i < numArgs; i++) {
            args[i + 1] = "invalid";

        }
        return args;
    }
}
