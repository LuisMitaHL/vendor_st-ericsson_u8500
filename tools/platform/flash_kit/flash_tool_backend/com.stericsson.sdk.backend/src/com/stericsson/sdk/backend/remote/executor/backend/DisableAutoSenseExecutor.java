package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;

/**
 * Executor for disabling auto sense
 * 
 * @author xolabju
 * 
 */
public class DisableAutoSenseExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(DisableAutoSenseExecutor.class);

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
            backendService.setAcceptEquipments(false);
        }
        return null;
    }
}
