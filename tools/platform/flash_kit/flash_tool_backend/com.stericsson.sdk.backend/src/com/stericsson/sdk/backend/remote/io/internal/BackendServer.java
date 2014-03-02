package com.stericsson.sdk.backend.remote.io.internal;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.SocketTimeoutException;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.io.BackendClientHandler;
import com.stericsson.sdk.backend.remote.io.BackendServerConnection;
import com.stericsson.sdk.backend.remote.io.ServerProperties;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;

/**
 * Class for setting up connections to remote clients
 * 
 * @author xolabju
 * 
 */
public final class BackendServer implements IConfigurationServiceListener {

    private static Logger logger = Logger.getLogger(BackendServer.class);

    private boolean isAccepting;

    private ServerSocket serverSocket;

    private static final int ACCEPT_CYCLE_MILLIS = 500;

    private int port;

    private Thread brpServerThread;

    /**
     * Constructor
     */
    public BackendServer() {
        isAccepting = false;
    }

    private void startServer() {
        port = ServerProperties.DEFAULT_PORT_NUMBER;
        String property = getPortProperty();
        if (property != null) {
            try {
                port = Integer.parseInt(property);
            } catch (NumberFormatException nfe) {
                logger.error("Failed to read property " + ServerProperties.PORT_PROPERTY_NAME + ". Using default port "
                    + ServerProperties.DEFAULT_PORT_NUMBER);
            }
        }
        isAccepting = true;
        brpServerThread = new Thread(new Runnable() {
            public void run() {

                try {
                    serverSocket = new ServerSocket(port);
                    serverSocket.setSoTimeout(ACCEPT_CYCLE_MILLIS);

                    logger.info("Started BRP Server on port " + port);
                } catch (IOException ioe) {
                    isAccepting = false;
                    logger.error("Failed to start BRP Server on port " + port + ": " + ioe.getMessage());
                }
                while (isAccepting) {
                    try {
                        BackendServerConnection client = new BackendServerConnection(serverSocket.accept());
                        logger.info("Accepted connection from " + client.getAddress());
                        new Thread(new BackendClientHandler(client), "ClientHandler @ " + client.getAddress()).start();
                    } catch (SocketTimeoutException ste) {
                        // ignore
                        ste.getMessage();
                    } catch (IOException ioe) {
                        logger.error("Failed to accept socket connection: " + ioe.getMessage());
                    }
                }
                try {
                    if (serverSocket != null) {
                        serverSocket.close();
                    }
                    serverSocket = null;
                } catch (IOException e) {
                    logger.debug("Exception when closing BRP Server socket: " + e.getMessage());
                }

            }
        }, "BRP Server @ port " + port);
        brpServerThread.start();
    }

    /**
     * Stops the server
     */
    public void stopServer() {
        logger.info("Stopping BRP server");
        isAccepting = false;
        try {
            if (brpServerThread != null) {
                brpServerThread.join();
                brpServerThread = null;
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void setEnabled(boolean enabled) {
        synchronized (this) {
            if (enabled != isAccepting) {
                if (enabled) {
                    startServer();
                } else {
                    stopServer();
                }
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord) {

        if (ConfigurationOption.MONITOR.equalsIgnoreCase(newRecord.getName())) {
            boolean enabled = false;

            if ("true".equalsIgnoreCase(newRecord.getValue(ConfigurationOption.MONITOR_ACCEPT_CLIENTS))) {
                enabled = true;
            }

            synchronized (this) {
                if (enabled != isAccepting) {
                    if (enabled) {
                        startServer();
                    } else {
                        stopServer();
                    }
                }
            }
        }
    }

    /**
     * 
     * @return key root
     */
    private String getPortProperty() {
        String keyRoot = System.getProperty(ServerProperties.PORT_PROPERTY_NAME);

        return keyRoot == null || keyRoot.trim().equals("") ? System.getenv(ServerProperties.PORT_PROPERTY_NAME)
            : keyRoot;
    }
}
