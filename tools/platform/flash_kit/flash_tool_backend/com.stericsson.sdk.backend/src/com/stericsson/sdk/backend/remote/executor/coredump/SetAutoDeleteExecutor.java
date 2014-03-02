package com.stericsson.sdk.backend.remote.executor.coredump;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;

/**
 * Remote command executor for auto delete of downloaded dump
 * 
 * @author esrimpa
 */
public class SetAutoDeleteExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(SetAutoDeleteExecutor.class);

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
        String setStatus = getCommand().getValue(AbstractCommand.PARAMETER_COREDUMP_SET_STATUS);
        if (backendService == null) {
            throw new ExecutionException("Cannot find IBackendService!");
        } else {
            boolean status = Boolean.parseBoolean(setStatus);
            boolean returnVal = backendService.setAutoDelete(status);
            if (returnVal) {
                message = "Automatic delete of newly downloaded dumps has been " + (status ? "enabled." : "disabled.");
            } else {
                message =
                    "To enable automatic delete of newly downloaded dump, you need to enable automatic download of new dump."
                        + " Refer command <set_auto_download>.";
            }
        }
        return message;
    }
}
