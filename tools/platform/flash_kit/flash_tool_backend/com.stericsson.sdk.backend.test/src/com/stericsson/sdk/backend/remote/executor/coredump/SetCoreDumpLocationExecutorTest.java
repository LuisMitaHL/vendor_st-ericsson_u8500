package com.stericsson.sdk.backend.remote.executor.coredump;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * Test class for SetCoreDumpLocationExecutor.
 * 
 * @author eolabor
 */
public class SetCoreDumpLocationExecutorTest extends TestCase {

    /**
     * Test SetCoreDumpLocation command.
     */
    @Test
    public void testSetSubscription() {
        SetCoreDumpLocationExecutor executor = new SetCoreDumpLocationExecutor();
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
        String completeString =
            CommandName.BACKEND_SET_CORE_DUMP_LOCATION.name() + AbstractCommand.DELIMITER + "somePath";
        return CommandFactory.createCommand(completeString);
    }

}
