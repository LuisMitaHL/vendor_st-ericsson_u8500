package com.stericsson.sdk.backend.remote.executor.backend;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.InvalidSyntaxException;


/**
 * @author Xxvs0002
 *
 */
public class SetLocalSigningExecutorTest extends TestCase{

    /**
     * 
     */
    @Test
    public void testLocalSigning(){
        SetLocalSigningExecutor setLocalSigningExecutor = new SetLocalSigningExecutor();
        try {
            setLocalSigningExecutor.setCommand(createCommand("TRUE"));
            String result = setLocalSigningExecutor.execute();

            assertNotNull(result);
        } catch (InvalidSyntaxException e) {
            assertTrue(e.getMessage(), false);
        } catch (ExecutionException e) {
            assertTrue(e.getMessage(), false);
        }
    }

    private AbstractCommand createCommand(String param) throws InvalidSyntaxException {
        String completeString = CommandName.BACKEND_SET_LOCAL_SIGNING  + AbstractCommand.DELIMITER + param;
        return CommandFactory.createCommand(completeString);
    }

    /**
     * 
     */
    @Test
    public void testLocalSigningWithException(){
        SetLocalSigningExecutor setLocalSigningExecutor = new SetLocalSigningExecutor();
        try {
            setLocalSigningExecutor.setCommand(createCommand("TEST"));
            String result = setLocalSigningExecutor.execute();

            assertNotNull(result);
            fail("Should not get here");
        } catch (InvalidSyntaxException e) {
            assertTrue(e.getMessage(), false);
        } catch (ExecutionException e) {
            e.getMessage();
        }
    }
}
