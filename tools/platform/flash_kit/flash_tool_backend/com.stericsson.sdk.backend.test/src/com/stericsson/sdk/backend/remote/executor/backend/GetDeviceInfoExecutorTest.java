/**
 * 
 */
package com.stericsson.sdk.backend.remote.executor.backend;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

import junit.framework.TestCase;

/**
 * @author mbocek01
 * 
 */
public class GetDeviceInfoExecutorTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testGetDeviceInfo() {
        GetDeviceInfoExecutor executor = new GetDeviceInfoExecutor();
        try {
            executor.setCommand(createCommand("USB0"));
            String result = executor.execute();
            assertNotNull(result);
        } catch (InvalidSyntaxException e) {
            assertTrue(e.getMessage(), false);
        } catch (ExecutionException e) {
            assertTrue(e.getMessage(), false);
        }
    }

    private AbstractCommand createCommand(String param) throws InvalidSyntaxException {
        String completeString = CommandName.BACKEND_GET_DEVICE_INFO.name() + AbstractCommand.DELIMITER + param;
        return CommandFactory.createCommand(completeString);
    }

}
