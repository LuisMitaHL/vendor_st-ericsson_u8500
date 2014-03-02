package com.stericsson.sdk.backend.remote.executor.backend;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

import junit.framework.TestCase;

/**
 * @author ezaptom
 *
 */
public class SetActiveProfileExecutorTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testSetActiveProfile() {
        SetActiveProfileExecutor executor = new SetActiveProfileExecutor();
        try {
            executor.setCommand(createCommand("TEST"));
            String result = executor.execute();

            assertNotNull(result);
        } catch (InvalidSyntaxException e) {
            assertTrue(e.getMessage(), false);
        } catch (ExecutionException e) {
            assertTrue(e.getMessage(), false);
        }
    }

    /**
     * 
     */
    @Test
    public void testIllegalProfileAlias() {
        SetActiveProfileExecutor executor = new SetActiveProfileExecutor();
        try {
            executor.setCommand(createCommand(" "));
            executor.execute();

            assertTrue("ExecutionException should be throwen", false);
        } catch (InvalidSyntaxException e) {
            assertTrue(e.getMessage(), false);
        } catch (ExecutionException e) {
            assertTrue(e.getMessage(), true);
        }
    }

    private AbstractCommand createCommand(String param) throws InvalidSyntaxException {
        String completeString = CommandName.BACKEND_SET_ACTIVE_PROFILE.name() + AbstractCommand.DELIMITER + param;
        return CommandFactory.createCommand(completeString);
    }
}
