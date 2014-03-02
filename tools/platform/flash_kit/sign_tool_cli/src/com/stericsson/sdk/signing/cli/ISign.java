package com.stericsson.sdk.signing.cli;

/**
 * @author xtomzap
 * 
 */
public interface ISign {

    /**
     * @param arguments
     *            input arguments
     * @return result
     * @throws Exception
     *             Exception
     */
    int execute(String[] arguments) throws Exception;
}
