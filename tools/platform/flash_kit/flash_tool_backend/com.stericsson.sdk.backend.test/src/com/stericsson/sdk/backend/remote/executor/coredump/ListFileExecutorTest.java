package com.stericsson.sdk.backend.remote.executor.coredump;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * Test cases for list file executor
 * 
 * @author esrimpa
 * 
 */
public class ListFileExecutorTest extends TestCase {

    ListFileExecutor executor = new ListFileExecutor();

    /**
     * Test Download dump
     */
    @Test
    public void testListFile() {
        try {
            executor.setCommand(createCommand());
            String result = executor.execute();
            assertNotNull(result);
        } catch (InvalidSyntaxException e) {
            assertTrue(e.getMessage(), false);
        } catch (ExecutionException e) {
            assertTrue(e.getMessage(), false);
        }
    }

    private AbstractCommand createCommand() throws InvalidSyntaxException {
        String completeString = CommandName.COREDUMP_LIST_FILE.name() + AbstractCommand.DELIMITER + "PortA";
        return CommandFactory.createCommand(completeString);
    }
}
