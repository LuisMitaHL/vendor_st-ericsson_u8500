/**
 * 
 */
package com.stericsson.sdk.backend.remote.executor.security;

import java.io.IOException;

import org.junit.Test;

import com.stericsson.sdk.backend.remote.io.BackendClientHandler;
import com.stericsson.sdk.backend.remote.io.IBackendServerConnection;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

import junit.framework.TestCase;

/**
 * @author cizovhel
 *
 */
public class BindPropertiesExecutorTest extends TestCase {
    /**
     * 
     */
    @Test
    public void testBindEquipmentProperties() {
        BindPropertiesExecutor executor = new BindPropertiesExecutor();
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
                CommandFactory.createCommand(CommandName.SECURITY_BIND_PROPERTIES.name()
                    + AbstractCommand.DELIMITER + "PortA");

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
