package com.stericsson.sdk.backend.remote.executor.coredump;

import java.io.File;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;

/**
 * Executor for setting the core dump location
 * 
 * @author olabor
 * 
 */
public class SetCoreDumpLocationExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(SetCoreDumpLocationExecutor.class);

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
        String path = getCommand().getValue(AbstractCommand.PARAMETER_PATH);
        path = new File(path).getAbsolutePath();
        if (backendService == null) {
            throw new ExecutionException("Cannot find IBackendService!");
        } else {
            backendService.setCoreDumpLocation(path);
            message = "Core dump location set to " + path;
        }
        return message;
    }
}
