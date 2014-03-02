package com.stericsson.sdk.backend.remote.executor.backend;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;

/**
 * Executor for triggering UART port.
 * 
 * @author pkutac01
 * 
 */
public class TriggerUARTPortExecutor extends ServerCommandExecutor {

    static final Logger log = Logger.getLogger(TriggerUARTPortExecutor.class);

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
     * 
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        IBackendService backendService = getBackendService();
        if (backendService == null) {
            throw new ExecutionException("Cannot find IBackendService!");
        } else {
            String portName = getCommand().getValue(AbstractCommand.PARAMETER_PORT_NAME);

            IConfigurationRecord config = new MESPConfigurationRecord();
            config.setName(ConfigurationOption.PORT_UART);
            config.setValue(ConfigurationOption.PORT_UART_ALIAS, portName);
            config.setValue(ConfigurationOption.PORT_UART_TRIGGER, String.valueOf(true));
            log.info("Trigger UART port: " + portName);
            backendService.notifyConfigurationChanged(null, config);
        }
        return null;
    }

}
