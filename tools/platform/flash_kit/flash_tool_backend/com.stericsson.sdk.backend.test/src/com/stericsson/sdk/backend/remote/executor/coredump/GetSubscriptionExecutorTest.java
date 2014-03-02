package com.stericsson.sdk.backend.remote.executor.coredump;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * Test class for GetSubscriptionExecutor.
 * 
 * @author qkarhed
 */
public class GetSubscriptionExecutorTest extends TestCase {

    /***
     * Test GetSubscription command.
     */
    @Test
    public void testGetSubscription() {
        GetSubscriptionExecutor executor = new GetSubscriptionExecutor();
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
            CommandName.COREDUMP_GET_SUBSCRIPTION.name() + AbstractCommand.DELIMITER + "127.0.0.1"
                + AbstractCommand.DELIMITER + "80";
        return CommandFactory.createCommand(completeString);
    }

}
