package com.stericsson.sdk.brp;

import java.io.IOException;

/**
 * Interface for a remote protocol command executor
 * 
 * @see IExecutionHandler
 * @see AbstractCommand
 * @author xolabju
 * 
 */
public interface ICommandExecutor {

    /**
     * Indicates that the execution completed sucessfully
     * 
     * @throws IOException
     *             on errors
     */
    void returnSuccess() throws IOException;

    /**
     * Indicates that the execution completed sucessfully
     * 
     * @param returnValue
     *            the additonal message to return together with the success response
     * @throws IOException
     *             on errors
     */
    void returnSuccess(String returnValue) throws IOException;

    /**
     * Indiicates that the command completed with errors
     * 
     * @param error
     *            the additonal message to return together with the error response
     * @throws IOException
     *             on errors
     */
    void returnError(String error) throws IOException;

    /**
     * Used to indicate the progress
     * 
     * @param percent
     *            percent completed
     * @param speed
     *            the current transmission speed in bytes / second
     * @throws IOException
     *             on errors
     */
    void progress(int percent, long speed) throws IOException;

    /**
     * Used to indicate the progress
     * 
     * @param percent
     *            percent completed
     * 
     * @throws IOException
     *             on errors
     */
    void progressPercent(int percent) throws IOException;

    /**
     * Called when the command is to be executed
     * 
     * @throws ExecutionException
     *             if the execution fails
     * @return the return value of the command or null if the command does not return anything
     */
    String execute() throws ExecutionException;

    /**
     * Cancels the operation if possible
     * 
     * @see #isCancellable()
     */
    void cancel();

    /**
     * 
     * @return true if the execution is to be cancelled, else false
     */
    boolean isCancelled();

    /**
     * Set the associated execution handler
     * 
     * @param handler
     *            the execution handler
     */
    void setExecutionHandler(IExecutionHandler handler);

    /**
     * Get the associated execution handler
     * 
     * @return the execution handler
     */
    IExecutionHandler getExecutionHandler();

    /**
     * Set the associated command
     * 
     * @param cmd
     *            the command
     */
    void setCommand(AbstractCommand cmd);

    /**
     * Get the associated command
     * 
     * @return the command
     */
    AbstractCommand getCommand();
}
