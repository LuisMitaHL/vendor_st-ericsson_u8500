package com.stericsson.sdk.backend.remote.executor.backend;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;
/**
 * @author ezaptom
 *
 */
public class GetActiveProfileExecutorTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testGetActiveProfile() {
        GetActiveProfileExecutor executor = new GetActiveProfileExecutor();
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
        String completeString = CommandName.BACKEND_GET_ACTIVE_PROFILE.name();
        return CommandFactory.createCommand(completeString);
    }
}
