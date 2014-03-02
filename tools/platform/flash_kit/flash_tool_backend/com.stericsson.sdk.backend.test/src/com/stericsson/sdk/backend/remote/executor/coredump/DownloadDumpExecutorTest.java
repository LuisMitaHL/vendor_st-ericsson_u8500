package com.stericsson.sdk.backend.remote.executor.coredump;

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
 * Test cases for download dump executor.
 * 
 * @author esrimpa
 * 
 */
public class DownloadDumpExecutorTest extends TestCase {

    DownloadDumpExecutor executor;

    /**
     * 
     */
    @Test
    public void testDownloadDumpExecutor() {
        executor = new DownloadDumpExecutor();

        executor.setExecutionHandler(new BackendClientHandler(new IBackendServerConnection() {
            public void write(String line) throws IOException {
            }

            public String readLine() throws IOException {
                return "test";
            }

            public String getAddress() {
                return "test";
            }

            public void close() {

            }
        }));

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
            CommandName.COREDUMP_DOWNLOAD_DUMP.name() + AbstractCommand.DELIMITER + "PortA" + AbstractCommand.DELIMITER
                + "Coredump_1" + AbstractCommand.DELIMITER + "PATH";
        return CommandFactory.createCommand(completeString);
    }
}
