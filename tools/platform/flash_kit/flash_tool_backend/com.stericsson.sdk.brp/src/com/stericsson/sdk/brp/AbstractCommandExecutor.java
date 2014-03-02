package com.stericsson.sdk.brp;

import java.io.IOException;

/**
 * Abstract class for a remote protocol server command executor
 * 
 * @author xolabju
 * 
 */
public abstract class AbstractCommandExecutor implements ICommandExecutor, Runnable {

    private boolean isCancelled;

    private IExecutionHandler executionHandler;

    private AbstractCommand command;

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        isCancelled = true;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelled() {
        return isCancelled;
    }

    /**
     * {@inheritDoc}
     */
    public abstract String execute() throws ExecutionException;

    /**
     * {@inheritDoc}
     */
    public AbstractCommand getCommand() {
        return command;
    }

    /**
     * {@inheritDoc}
     */
    public IExecutionHandler getExecutionHandler() {
        return executionHandler;
    }

    /**
     * {@inheritDoc}
     */
    public abstract void progress(int percent, long speed) throws IOException;

    /**
     * {@inheritDoc}
     */
    public abstract void progressPercent(int percent) throws IOException;

    /**
     * {@inheritDoc}
     */
    public abstract void returnError(String error) throws IOException;

    /**
     * {@inheritDoc}
     */
    public abstract void returnSuccess() throws IOException;

    /**
     * {@inheritDoc}
     */
    public abstract void returnSuccess(String returnValue) throws IOException;

    /**
     * {@inheritDoc}
     */
    public void setCommand(AbstractCommand cmd) {
        command = cmd;
    }

    /**
     * {@inheritDoc}
     */
    public void setExecutionHandler(IExecutionHandler handler) {
        executionHandler = handler;

    }

    /**
     * {@inheritDoc}
     */
    public void run() {
        try {
            String execute = execute();
            if (execute == null || execute.trim().equals("")) {
                returnSuccess();
            } else {
                returnSuccess(execute);
            }
        } catch (IOException e) {
            if (executionHandler != null) {
                executionHandler.fatalError(getCommand(), e.getMessage());
            }
        } catch (ExecutionException ee) {
            try {
                returnError(ee.getMessage());
            } catch (IOException e) {
                if (executionHandler != null) {
                    executionHandler.fatalError(getCommand(), e.getMessage());
                }
            }
        }
    }

}
