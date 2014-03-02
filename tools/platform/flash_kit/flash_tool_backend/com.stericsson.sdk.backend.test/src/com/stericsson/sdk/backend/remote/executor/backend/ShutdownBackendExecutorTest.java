package com.stericsson.sdk.backend.remote.executor.backend;

import java.io.IOException;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.backend.remote.io.BackendClientHandler;
import com.stericsson.sdk.backend.remote.io.IBackendServerConnection;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * @author xolabju
 * 
 */
public class ShutdownBackendExecutorTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testShutdownBackend() {
        ShutdownBackendExecutor executor = new ShutdownBackendExecutor();
        executor.setExecutionHandler(new BackendClientHandler(new IBackendServerConnection() {

            public void write(String line) throws IOException {

            }

            public String readLine() throws IOException {
                return null;
            }

            public String getAddress() {
                return null;
            }

            public void close() {

            }
        }));
        try {
            executor.setCommand(createCommand("TEST"));
            String result = executor.execute();

            assertNull(result);
        } catch (InvalidSyntaxException e) {
            assertTrue(e.getMessage(), false);
        } catch (ExecutionException e) {
            assertTrue(e.getMessage(), false);
        }
    }

    private AbstractCommand createCommand(String param) throws InvalidSyntaxException {
        String completeString = CommandName.BACKEND_SHUTDOWN_BACKEND.name() + AbstractCommand.DELIMITER + param;
        return CommandFactory.createCommand(completeString);
    }
}
