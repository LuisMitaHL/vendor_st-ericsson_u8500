package com.stericsson.sdk.backend.remote.executor.coredump;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * Test class for GetCoreDumpLocationExecutor.
 * 
 * @author eolabor
 */
public class GetCoreDumpLocationExecutorTest extends TestCase {

    /**
     * Test GetCoreDumpLocation command.
     */
    @Test
    public void testSetSubscription() {
        GetCoreDumpLocationExecutor executor = new GetCoreDumpLocationExecutor();
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
        String completeString = CommandName.BACKEND_GET_CORE_DUMP_LOCATION.name();
        return CommandFactory.createCommand(completeString);
    }

}
