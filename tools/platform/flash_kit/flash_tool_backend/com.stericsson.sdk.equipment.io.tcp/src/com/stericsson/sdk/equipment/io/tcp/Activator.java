package com.stericsson.sdk.equipment.io.tcp;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;

import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;

/**
 * @author xtomzap
 * 
 */
public class Activator implements BundleActivator, IConfigurationServiceListener {

    private Logger logger = Logger.getLogger(Activator.class.getName());

    private TCPIPServer server = null;

    /**
     * 
     * {@inheritDoc}
     */
    public void start(BundleContext context) throws Exception {
        context.registerService(IConfigurationServiceListener.class.getName(), this, null);
    }

    /**
     * 
     * {@inheritDoc}
     */
    public void stop(BundleContext context) throws Exception {
        stopServer();
    }

    private void stopServer() {
        if (server != null) {
            server.stop();
            server = null;
        }
    }

    /**
     * Listener for configuration change
     * 
     * @param service
     *            configuration service
     * @param oldRecord
     *            Old configuration Record
     * @param newRecord
     *            New configuration Record
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {

        if (newRecord.getName().equalsIgnoreCase(ConfigurationOption.ETHERNET_PORT)) {
            String enabled = newRecord.getValue(ConfigurationOption.ETHERNET_PORT_ENABLED);
            if (enabled.equalsIgnoreCase("true")) {
                String portString = newRecord.getValue(ConfigurationOption.ETHERNET_PORT_NUMBER);
                try {
                    int portInt = Integer.parseInt(portString);
                    stopServer();
                    server = new TCPIPServer(portInt);
                    new Thread(server).start();

                } catch (Throwable t) {
                    logger.error("The ethernet (tcp) port specification in configuration file is invalid.", t);
                }
            } else {
                stopServer();
            }
        }
    }
}
