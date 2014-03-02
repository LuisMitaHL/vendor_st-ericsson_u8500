package com.stericsson.sdk.equipment;

import java.io.IOException;

/**
 * @author xtomzap
 *
 */
public interface IRuntimeExecutor {

    /**
     * @param command to execute
     * @return exit value of the process
     * @throws IOException ex
     * @throws InterruptedException ex 
     */
    int execute(String command) throws IOException, InterruptedException;
}
