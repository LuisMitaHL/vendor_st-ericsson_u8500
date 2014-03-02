package com.stericsson.sdk.brp;

/**
 * Interface for an execution handler
 * 
 * @see ICommandExecutor
 * @see AbstractCommand
 * @author xolabju
 * 
 */
public interface IExecutionHandler {

    /**
     * Called when an execution is completed, either with success or with errors
     * 
     * @param executor
     *            the executor
     */
    void done(ICommandExecutor executor);

    /**
     * Called when the execution failed to report the result
     * 
     * @param command
     *            the executed command
     * @param errorMessage
     *            the error message
     */
    void fatalError(AbstractCommand command, String errorMessage);

}
