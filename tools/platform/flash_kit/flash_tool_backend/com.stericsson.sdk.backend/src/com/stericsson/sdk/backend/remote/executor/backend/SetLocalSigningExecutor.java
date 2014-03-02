package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;

/**
 * Executor for setting local signing in backend
 * 
 * @author xtomzap
 * 
 */
public class SetLocalSigningExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(SetLocalSigningExecutor.class);

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        boolean localSigningValue = getLocalSigningValue();

        IBackendService service = getBackendService();
        service.setLocalSigning(localSigningValue);

        return "Local signing set to: " + localSigningValue;
    }

    private boolean getLocalSigningValue() throws ExecutionException {
        String localSigningString = getCommand().getValue(AbstractCommand.PARAMETER_LOCAL_SIGNING);
        if (localSigningString == null || localSigningString.trim().equals("")
            || (!localSigningString.trim().equalsIgnoreCase(Boolean.TRUE.toString())
                && !localSigningString.trim().equalsIgnoreCase(Boolean.FALSE.toString()))) {
            log.debug("Illegal local signing value specified.");
            throw new ExecutionException("Illegal local signing value specified.");
        }

        return Boolean.parseBoolean(localSigningString.trim());
    }

    private IBackendService getBackendService() throws ExecutionException {
        ServiceReference[] references = null;

        try {
            references = Activator.getBundleContext().getAllServiceReferences(IBackendService.class.getName(), null);
        } catch (InvalidSyntaxException e) {
            log.error(e.getMessage());
        }

        if ((references == null) || references.length == 0) {
            throw new ExecutionException("Cannot find IBackendService!");
        }

        return (IBackendService) Activator.getBundleContext().getService(references[0]);
    }
}
