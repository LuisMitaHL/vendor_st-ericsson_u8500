package com.stericsson.sdk.equipment;

import java.io.IOException;

/**
 * @author xtomzap
 *
 */
public class RuntimeExecutorMock implements IRuntimeExecutor {

    /**
     * {@inheritDoc}
     */
    public int execute(String pCommand) throws IOException, InterruptedException {
        return 0;
    }
}
