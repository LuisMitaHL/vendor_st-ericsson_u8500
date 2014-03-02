package com.stericsson.sdk.backend.remote.executor.backend;

import java.io.IOException;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.common.EnvironmentProperties;

/**
 * Executor for shutting down the backend
 * 
 * @author xolabju
 * 
 */
public class ShutdownBackendExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(ShutdownBackendExecutor.class);

    private IBackendService getBackendService() {
        ServiceReference[] references = null;
        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            log.error(e.getMessage());
        }

        if ((references == null) || (references.length == 0)) {
            return null;
        }

        return (IBackendService) Activator.getBundleContext().getService(references[0]);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        IBackendService backendService = getBackendService();
        if (backendService == null) {
            throw new ExecutionException("Cannot find IBackendService!");
        } else {
            String message = getCommand().getValue(AbstractCommand.PARAMETER_MESSAGE);
            log.info("Shutting down backend service: " + message);
            try {
                returnSuccess("Shutdown of backend initiated");
            } catch (IOException e) {
                log.error("Failed to report back to client: " + e.getMessage());
            }
            // avoid crashing the test run
            if (!EnvironmentProperties.isRunningInTestMode()) {
                backendService.shutdown();
            } else {
                log.info("running test version");
            }
        }
        return null;
    }
}
