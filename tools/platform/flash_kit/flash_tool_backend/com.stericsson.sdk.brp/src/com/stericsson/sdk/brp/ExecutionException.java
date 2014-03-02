package com.stericsson.sdk.brp;

/**
 * Exception that will be thrown when a command fails to execute
 * 
 * @author xolabju
 * 
 */
public class ExecutionException extends Exception {

    private static final long serialVersionUID = -7498836699749007326L;

    /**
     * Constructor
     * 
     * @param msg
     *            exception message
     */
    public ExecutionException(String msg) {
        super(msg);
    }

    /**
     * @param t .
     */
    public ExecutionException(Throwable t){
        super(t);
    }
}
