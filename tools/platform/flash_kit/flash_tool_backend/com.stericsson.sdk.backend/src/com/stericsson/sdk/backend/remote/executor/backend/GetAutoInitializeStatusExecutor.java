package com.stericsson.sdk.backend.remote.executor.backend;

import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.backend.GetAutoInitializeStatus;

/**
 * Executor for getting auto initialize status
 * 
 * @author xolabju
 * 
 */
public class GetAutoInitializeStatusExecutor extends ServerCommandExecutor {

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
            return GetAutoInitializeStatus.AUTO_INITIALIZE_STATUS_ON;
    }
}
