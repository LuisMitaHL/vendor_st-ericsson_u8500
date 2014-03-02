package com.stericsson.sdk.backend.remote.executor.backend;

import java.io.IOException;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;

/**
 * Executor for reloading backend configuration
 * 
 * @author xolabju
 * 
 */
public class ReloadConfigurationExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(ReloadConfigurationExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        IBackendService backendService = getBackendService();
        if (backendService == null) {
            log.error("Cannot reload configuration, no Backend service found.");
            throw new ExecutionException("Cannot reload configuration, no Backend service found.");
        }

        try {
            backendService.loadConfiguration();
        } catch (IOException e) {
            throw new ExecutionException(e.getMessage());
        } catch (NullPointerException e) {
            throw new ExecutionException(e.getMessage());
        }
        return "Reload configuration completed.";
    }

    private IBackendService getBackendService() throws ExecutionException {
        ServiceReference[] references = null;
        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            log.error(e.getMessage());
            throw new ExecutionException(e.getMessage());
        }

        if (references == null || references.length == 0) {
            log.error("references[] are null or 0");
            return null;
        }

        return (IBackendService) Activator.getBundleContext().getService(references[0]);
    }
}
