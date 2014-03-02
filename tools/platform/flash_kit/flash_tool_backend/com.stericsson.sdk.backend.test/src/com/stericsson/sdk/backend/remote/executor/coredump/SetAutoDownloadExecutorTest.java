package com.stericsson.sdk.backend.remote.executor.coredump;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * Test class for SetAutoDownloadExecutor.
 * 
 * @author eolabor
 */
public class SetAutoDownloadExecutorTest extends TestCase {

    /**
     * Test SetAutoDownload command.
     */
    @Test
    public void testSetSubscription() {
        SetAutoDownloadExecutor executor = new SetAutoDownloadExecutor();
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
        String completeString = CommandName.COREDUMP_SET_AUTO_DOWNLOAD.name() + AbstractCommand.DELIMITER + "true";
        return CommandFactory.createCommand(completeString);
    }

}
