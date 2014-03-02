package com.stericsson.sdk.backend.remote.executor.coredump;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;

/**
 * Remote command executor get status of auto download of new dump
 * 
 * @author eolabor
 */
public class GetAutoDownloadExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(GetAutoDownloadExecutor.class);

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

        String message = null;
        if (backendService == null) {
            throw new ExecutionException("Cannot find IBackendService!");
        } else {
            boolean status = backendService.getAutoDownload();
            message = (status ? "enabled." : "disabled.");
        }
        return message;
    }
}
