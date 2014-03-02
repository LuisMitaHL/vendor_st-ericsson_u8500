package com.stericsson.sdk.backend.remote.executor.flash;

import java.io.IOException;

import junit.framework.TestCase;

import com.stericsson.sdk.backend.remote.io.BackendClientHandler;
import com.stericsson.sdk.backend.remote.io.IBackendServerConnection;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * @author xdancho
 * 
 */
public class EraseAreaExecutorTest extends TestCase {

    /**
     * 
     */
    public void testDumpAreaExecutor() {
        DumpAreaExecutor executor = new DumpAreaExecutor();

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

        AbstractCommand ac = null;
        try {
            ac =
                CommandFactory.createCommand(CommandName.FLASH_ERASE_AREA.name() + AbstractCommand.DELIMITER + "PortA"
                    + AbstractCommand.DELIMITER + "/flash0/" + AbstractCommand.DELIMITER + "0"
                    + AbstractCommand.DELIMITER + "0x1000");

        } catch (InvalidSyntaxException e) {
            fail(e.getMessage());
        }
        executor.setCommand(ac);

        try {
            assertNotNull(executor.execute());
        } catch (ExecutionException e) {
            fail(e.getMessage());
        }

    }

}
