package com.stericsson.sdk.brp.test;

import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * 
 * @author xolabju
 * 
 */
public class CommandFactoryTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testCreateInvalidCommand() {
        String invalidCommand = "InvalidCommand;arg1;arg2;arg3";
        try {
            assertNull(CommandFactory.createCommand(invalidCommand));

        } catch (InvalidSyntaxException ise) {
            fail();
        }

    }

    /**
     * 
     */
    @Test
    public void testInvalidSyntaxException() {
        String[] invalidParams = createArguments(8);
        try {
            CommandFactory.createCommand(CommandName.BACKEND_ENABLE_AUTO_SENSE, invalidParams);
            fail();
        } catch (InvalidSyntaxException ise) {
            System.out.println(ise.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateCommand() {
        AbstractCommand command = null;
        for (CommandName cmdName : CommandName.values()) {
            try {
                command = CommandFactory.createCommand(cmdName);
                if (command == null) {
                    throw new Exception("Command is null");
                }
                assertNotNull("No command class defined for " + cmdName.name(), cmdName);
                assertEquals(cmdName.name(), command.getCommandName().name());
                AbstractCommand commandWithArgs =
                    CommandFactory.createCommand(cmdName, createArguments(command.getParameterNames() == null ? 0
                        : command.getParameterNames().length));
                if (commandWithArgs == null) {
                    throw new Exception("Command with args is null");
                }
                assertNotNull(commandWithArgs);
                assertNotNull(commandWithArgs.toString());
                assertTrue(Arrays.equals(command.getParameterNames(), commandWithArgs.getParameterNames()));
            } catch (Exception e) {
                fail(e.getMessage());
            }
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateCancelCommand() {
        AbstractCommand command = null;
        for (CommandName cmdName : CommandName.values()) {
            command = CommandFactory.createCommand(cmdName);
            if (command != null && command.isCancellable()) {
                test(command, cmdName);
            } else if (command != null) {
                try {
                    CommandFactory.createCancelCommand(cmdName, createArguments(command.getParameterNames() == null ? 0
                        : command.getParameterNames().length));
                    fail("Cancel should not be allowed");
                } catch (InvalidSyntaxException e) {
                    e.getMessage();
                }
            } else {
                fail("Command is null");
            }
        }
    }

    private void test(AbstractCommand command, CommandName cmdName) {
        try {
            AbstractCommand cmd1 =
                CommandFactory.createCommand(cmdName, createArguments(command.getParameterNames() == null ? 0
                    : command.getParameterNames().length));
            if (cmd1 == null) {
                throw new Exception("Command 1 is null");
            }
            AbstractCommand cmd2 =
                CommandFactory.createCancelCommand(cmd1.getCommandName(), createArguments(command
                    .getParameterNames() == null ? 0 : command.getParameterNames().length));
            if (cmd2 == null) {
                throw new Exception("Command 2 is null");
            }
            assertEquals(cmd2.getCommandString(), cmd1.getCommandString() + AbstractCommand.DELIMITER
                + AbstractCommand.CANCEL);
            AbstractCommand cmd3 =
                CommandFactory.createCancelCommand(cmd1.getCommandString() + AbstractCommand.DELIMITER);
            if (cmd3 == null) {
                throw new Exception("Command 3 is null");
            }
            assertEquals(cmd3.getCommandString(), cmd1.getCommandString() + AbstractCommand.DELIMITER
                + AbstractCommand.CANCEL);

        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    private String[] createArguments(int numArgs) {
        String[] args = new String[numArgs];
        for (int i = 0; i < numArgs; i++) {
            args[i] = "arg" + i;
        }
        return args;
    }
}
