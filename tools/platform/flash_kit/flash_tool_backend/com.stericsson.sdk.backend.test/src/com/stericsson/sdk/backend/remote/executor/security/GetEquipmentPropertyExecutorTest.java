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
 * @author xhelciz
 * 
 */
public class GetEquipmentPropertyExecutorTest extends TestCase {

    static final String PROPERTY_NAME = "0x01";

    /**
     * 
     */
    @Test
    public void testGetEquipmentProperty() {
        GetEquipmentPropertyExecutor executor = new GetEquipmentPropertyExecutor();
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
                CommandFactory.createCommand(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name()
                    + AbstractCommand.DELIMITER + "PortA" + AbstractCommand.DELIMITER + PROPERTY_NAME);

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
