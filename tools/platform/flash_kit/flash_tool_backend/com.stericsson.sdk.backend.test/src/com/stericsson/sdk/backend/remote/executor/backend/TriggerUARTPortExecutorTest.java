package com.stericsson.sdk.backend.remote.executor.backend;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;

import junit.framework.TestCase;


/**
 * @author Xxvs0002
 *
 */
public class TriggerUARTPortExecutorTest extends TestCase{

    /**
     * 
     */
    @Test
    public void testSetTriggerUart(){
        TriggerUARTPortExecutor executor = new TriggerUARTPortExecutor();
        try {
            executor.setCommand(createCommand("UART0"));
            String result = executor.execute();

            assertNull(result);
        } catch (InvalidSyntaxException e) {
            assertTrue(e.getMessage(), false);
        } catch (ExecutionException e) {
            assertTrue(e.getMessage(), false);
        }
    }

    private AbstractCommand createCommand(String param) throws InvalidSyntaxException {
        String completeString = CommandName.BACKEND_TRIGGER_UART_PORT.name() + AbstractCommand.DELIMITER + param;
        return CommandFactory.createCommand(completeString);
    }
}
