package com.stericsson.sdk.brp.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.InvalidSyntaxException;
import com.stericsson.sdk.brp.ServerResponse;

/**
 * 
 * @author xolabju
 * 
 */
public class AbstractCommandTest extends TestCase {
    AbstractCommand command = null;

    final String[] parameters = new String[] {
        "param1", "param2", "param3"};

    final String[] arguments = new String[] {
        "arg1", "arg2", "arg3"};

    final CommandName commandName = CommandName.BACKEND_DISABLE_AUTO_INITIALIZE;

    final String completeCommand =
        commandName.name() + AbstractCommand.DELIMITER + arguments[0] + AbstractCommand.DELIMITER + arguments[1]
            + AbstractCommand.DELIMITER + arguments[2];

    /**
     * 
     */
    @Test
    public void testAbstractCommand() {
        try {
            command = new AbstractCommand(completeCommand) {

                @Override
                public boolean isCancellable() {
                    return false;
                }

                @Override
                public String[] getParameterNames() {
                    return parameters;
                }

                @Override
                public CommandName getCommandName() {
                    return commandName;
                }
            };
            assertFalse(command.isCancelCommand());
        } catch (InvalidSyntaxException ise) {
            fail(ise.getMessage());
        }
        for (int i = 0; i < parameters.length; i++) {
            assertEquals(command.getValue(parameters[i]), arguments[i]);
        }

        try {
            command = new AbstractCommand(commandName, arguments) {

                @Override
                public boolean isCancellable() {
                    return false;
                }

                @Override
                public String[] getParameterNames() {
                    return parameters;
                }

                @Override
                public CommandName getCommandName() {
                    return commandName;
                }
            };
        } catch (InvalidSyntaxException ise) {
            fail(ise.getMessage());
        }
        for (int i = 0; i < parameters.length; i++) {
            assertEquals(command.getValue(parameters[i]), arguments[i]);
        }

    }

    /**
     * 
     */
    @Test
    public void testCancellableAbstractCommand() {
        try {
            command = new AbstractCommand(completeCommand + AbstractCommand.DELIMITER + AbstractCommand.CANCEL) {

                @Override
                public boolean isCancellable() {
                    return true;
                }

                @Override
                public String[] getParameterNames() {
                    return parameters;
                }

                @Override
                public CommandName getCommandName() {
                    return commandName;
                }
            };
            assertTrue(command.isCancelCommand());
        } catch (InvalidSyntaxException ise) {
            fail("Should not get here: " + ise.getMessage());
        }

    }

    /**
     * 
     */
    @Test
    public void testNonCancellableAbstractCommand() {
        try {
            command = new AbstractCommand(completeCommand + AbstractCommand.DELIMITER + AbstractCommand.CANCEL) {

                @Override
                public boolean isCancellable() {
                    return false;
                }

                @Override
                public String[] getParameterNames() {
                    return parameters;
                }

                @Override
                public CommandName getCommandName() {
                    return commandName;
                }
            };
            fail("CANCEL should not be allowed");
        } catch (InvalidSyntaxException ise) {
            System.out.println(ise.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testServerResponses() {
        for (ServerResponse response : ServerResponse.values()) {
            assertNotNull(response);
        }
    }
}
