package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.backend.GetAutoSenseStatus;

/**
 * Executor for getting auto sense status
 * 
 * @author xolabju
 * 
 */
public class GetAutoSenseStatusExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(GetAutoSenseStatusExecutor.class);

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
        } 
        if (backendService.isAcceptingEquipments()) {
            return GetAutoSenseStatus.AUTO_SENSE_STATUS_ON;
        }
        return GetAutoSenseStatus.AUTO_SENSE_STATUS_OFF;
    }
}
