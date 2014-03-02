package com.stericsson.sdk.brp.test;

import java.io.IOException;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.AbstractCommandExecutor;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.ICommandExecutor;
import com.stericsson.sdk.brp.IExecutionHandler;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * 
 * @author xolabju
 * 
 */
public class AbstractCommandExecutorTest extends TestCase {
    AbstractCommand command = null;

    final String[] parameters = new String[] {
        "param1", "param2", "param3"};

    final String[] arguments = new String[] {
        "arg1", "arg2", "arg3"};

    final CommandName commandName = CommandName.BACKEND_DISABLE_AUTO_INITIALIZE;

    final String completeCommand =
        commandName.name() + AbstractCommand.DELIMITER + arguments[0] + AbstractCommand.DELIMITER + arguments[1]
            + AbstractCommand.DELIMITER + arguments[2];

    /**
     * 
     */
    @Test
    public void testReturnValue() {

        final String value = "TestValue";

        AbstractCommandExecutor exec = new AbstractCommandExecutor() {

            @Override
            public void returnSuccess(String returnValue) throws IOException {
                assertEquals(value, returnValue);

            }

            @Override
            public void returnSuccess() throws IOException {

            }

            @Override
            public void returnError(String error) throws IOException {

            }

            @Override
            public void progress(int percent, long speed) throws IOException {

            }

            @Override
            public String execute() throws ExecutionException {
                return value;
            }

            @Override
            public void progressPercent(int pPercent) throws IOException {

            }
        };

        Thread t = new Thread(exec);
        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

    }

    /**
     * 
     */
    @Test
    public void testNoReturnValue() {

        AbstractCommandExecutor exec = new AbstractCommandExecutor() {

            @Override
            public void returnSuccess(String returnValue) throws IOException {
                fail();
            }

            @Override
            public void returnSuccess() throws IOException {
            }

            @Override
            public void returnError(String error) throws IOException {

            }

            @Override
            public void progress(int percent, long speed) throws IOException {

            }

            @Override
            public String execute() throws ExecutionException {
                return null;
            }

            @Override
            public void progressPercent(int pPercent) throws IOException {

            }
        };

        Thread t = new Thread(exec);
        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

    }

    /**
     * 
     */
    @Test
    public void testExecutionFailed() {
        final String errorMsg = "TestError";

        AbstractCommandExecutor exec = new AbstractCommandExecutor() {

            @Override
            public void returnSuccess(String returnValue) throws IOException {
                fail();
            }

            @Override
            public void returnSuccess() throws IOException {
                fail();
            }

            @Override
            public void returnError(String error) throws IOException {
                assertEquals(errorMsg, error);
            }

            @Override
            public void progress(int percent, long speed) throws IOException {

            }

            @Override
            public String execute() throws ExecutionException {
                throw new ExecutionException(errorMsg);
            }

            @Override
            public void progressPercent(int pPercent) throws IOException {

            }
        };

        Thread t = new Thread(exec);
        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testReturnErrorFailed() {
        final String errorMsg = "TestError";
        IExecutionHandler handler = null;
        final CommandName cmdName = CommandName.BACKEND_DISABLE_AUTO_INITIALIZE;
        AbstractCommand cmd = new AbstractCommand() {

            @Override
            public boolean isCancellable() {
                return false;
            }

            @Override
            public String[] getParameterNames() {
                return null;
            }

            @Override
            public CommandName getCommandName() {
                return cmdName;
            }
        };

        try {
            cmd.setCommand(cmdName.name());
        } catch (InvalidSyntaxException e1) {
            fail(e1.getMessage());
        }

        handler = new IExecutionHandler() {

            public void fatalError(AbstractCommand cmd, String errorMessage) {
                assertEquals(cmdName, cmd.getCommandName());
                assertEquals(errorMsg, errorMessage);

            }

            public void done(ICommandExecutor executor) {
                fail();
            }
        };

        AbstractCommandExecutor exec = new AbstractCommandExecutor() {

            @Override
            public void returnSuccess(String returnValue) throws IOException {
                fail();
            }

            @Override
            public void returnSuccess() throws IOException {
                fail();
            }

            @Override
            public void returnError(String error) throws IOException {
                throw new IOException(errorMsg);
            }

            @Override
            public void progress(int percent, long speed) throws IOException {

            }

            @Override
            public String execute() throws ExecutionException {
                throw new ExecutionException(errorMsg);
            }

            @Override
            public void progressPercent(int pPercent) throws IOException {

            }
        };

        exec.setCommand(cmd);
        exec.setExecutionHandler(handler);

        Thread t = new Thread(exec);
        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testReturnSuccessFailed() {
        final String errorMsg = "TestError";
        IExecutionHandler handler = null;
        final CommandName cmdName = CommandName.BACKEND_DISABLE_AUTO_INITIALIZE;
        AbstractCommand cmd = new AbstractCommand() {

            @Override
            public boolean isCancellable() {
                return false;
            }

            @Override
            public String[] getParameterNames() {
                return null;
            }

            @Override
            public CommandName getCommandName() {
                return cmdName;
            }
        };

        try {
            cmd.setCommand(cmdName.name());
        } catch (InvalidSyntaxException e1) {
            fail(e1.getMessage());
        }

        handler = new IExecutionHandler() {

            public void fatalError(AbstractCommand cmd, String errorMessage) {
                assertEquals(cmdName, cmd.getCommandName());
                assertEquals(errorMsg, errorMessage);

            }

            public void done(ICommandExecutor executor) {
                fail();
            }
        };

        AbstractCommandExecutor exec = new AbstractCommandExecutor() {

            @Override
            public void returnSuccess(String returnValue) throws IOException {
                fail();
            }

            @Override
            public void returnSuccess() throws IOException {
                throw new IOException(errorMsg);
            }

            @Override
            public void returnError(String error) throws IOException {
                fail();
            }

            @Override
            public void progress(int percent, long speed) throws IOException {

            }

            @Override
            public String execute() throws ExecutionException {
                return null;
            }

            @Override
            public void progressPercent(int pPercent) throws IOException {

            }
        };

        exec.setCommand(cmd);
        exec.setExecutionHandler(handler);

        Thread t = new Thread(exec);
        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testCommandCompleted() {
        IExecutionHandler handler = null;
        final CommandName cmdName = CommandName.BACKEND_DISABLE_AUTO_INITIALIZE;
        AbstractCommand cmd = new AbstractCommand() {

            @Override
            public boolean isCancellable() {
                return false;
            }

            @Override
            public String[] getParameterNames() {
                return null;
            }

            @Override
            public CommandName getCommandName() {
                return cmdName;
            }
        };

        try {
            cmd.setCommand(cmdName.name());
        } catch (InvalidSyntaxException e1) {
            fail(e1.getMessage());
        }

        final AbstractCommandExecutor exec = new AbstractCommandExecutor() {

            @Override
            public void returnSuccess(String returnValue) throws IOException {
            }

            @Override
            public void returnSuccess() throws IOException {
            }

            @Override
            public void returnError(String error) throws IOException {
            }

            @Override
            public void progress(int percent, long speed) throws IOException {

            }

            @Override
            public String execute() throws ExecutionException {
                return null;
            }

            @Override
            public void progressPercent(int pPercent) throws IOException {

            }
        };

        handler = new IExecutionHandler() {

            public void fatalError(AbstractCommand cmd, String errorMessage) {
                fail();
            }

            public void done(ICommandExecutor executor) {
                assertNotNull(executor);
                assertEquals(exec, executor);
            }
        };

        exec.setCommand(cmd);
        exec.setExecutionHandler(handler);
        assertNotNull(exec.getExecutionHandler());
        assertFalse(exec.isCancelled());
        exec.cancel();
        assertTrue(exec.isCancelled());

        Thread t = new Thread(exec);
        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
